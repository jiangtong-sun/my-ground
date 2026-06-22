#include "QQuickVideoView.h"
#include "log_category.h"

#include <QOpenGLPixelTransferOptions>
#include <QQuickWindow>

void QQuickVideoView::registerType() {
    qmlRegisterType<QQuickVideoView>("QQuickVideoView", 1, 0, "QQuickVideoView");
}

QQuickVideoView::QQuickVideoView(QQuickItem *parent) : QQuickFramebufferObject(parent) {
    _videoWidth = 1;
    _videoHeight = 1;
    _videoReady = false;
    _enableDevice = false;

    controller.setMediaState([=](bool state) { notifyVideoState(state); });

    reconn_timer.setSingleShot(true);
    connect(&reconn_timer, &QTimer::timeout, this, &QQuickVideoView::scheduleReconnect);

    connect(&controller, &DecoderController::videoInfoReady, this,
            &QQuickVideoView::onVideoInfoReady);
    _updateTimer = startTimer(1000 / 60);
}

void QQuickVideoView::onVideoInfoReady(int width, int height) {
    if (_videoWidth != width) {
        _videoWidth = width;
        _videoReady = true;
    }

    if (_videoHeight != height) {
        _videoHeight = height;
        _videoReady = true;
    }
}

void QQuickVideoView::setUrl(const QString &url) {
    QString cleanUrl = url.trimmed();
    if (!cleanUrl.isEmpty() && _url != cleanUrl) {
        _url = cleanUrl;
        emit urlChanged();
    }
}

void QQuickVideoView::notifyVideoState(const bool datas) { emit vidoeStateChanged(datas); }

QQuickVideoView::~QQuickVideoView() {
    killTimer(_updateTimer);
    reconn_timer.stop();
    closeStream();
}

QQuickFramebufferObject::Renderer *QQuickVideoView::createRenderer() const {
    COMMON_INFO("create GUI Renderer object");
    return new VideoViewRenderer(_enableDevice, nullptr);
}

void QQuickVideoView::openStream(bool cpu) {
    if (!_url.isEmpty()) {
        _videoWidth = 1;
        _videoHeight = 1;
        controller.openMedia(_url, cpu);
    }
}

void QQuickVideoView::setRWTimeout(int timeout) { controller.setTimeout(timeout); }

void QQuickVideoView::closeStream() {
    COMMON_INFO("close media {}", _url.toStdString());
    _videoReady = false; //
    controller.closeMedia();
}

void QQuickVideoView::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);
    QQuickItem::update();
}

void QQuickVideoView::scheduleReconnect() {
    closeStream();
    if (_videoparam & 0xff) {
        openStream((_videoparam >> 8) & 0xff);
    }
    _videoparam = 0;
}

void QQuickVideoView::replayStream(bool cpumode, bool enable) {
    COMMON_INFO("replay stream, cpumode: {}, enable: {}", cpumode, enable);
    _videoparam = (cpumode << 8) | enable;
    reconn_timer.start(100);
}

// void VideoViewRenderer::resize(int width, int height)
// {
//     glViewport(0, 0, width, height);
//     float bottom = -1.0f;
//     float top	 = 1.0f;
//     float n		 = 1.0f;
//     float f		 = 100.0f;
//     mProjectionMatrix.setToIdentity();
//     mProjectionMatrix.frustum(-1.0, 1.0, bottom, top, n, f);
// }

static void safeDeleteTexture(QOpenGLTexture *texture) {
    if (texture) {
        if (texture->isBound()) {
            texture->release();
        }

        if (texture->isCreated()) {
            texture->destroy();
        }

        delete texture;
        texture = nullptr;
    }
}

void VideoViewRenderer::deinitialize() {
#if defined(ENABLE_CUDA_DEVICE)
    if (cu_context != nullptr) {
        cuCtxDestroy(cu_context);
    }
#endif
    glDeleteBuffers(1, &_y_cache);
    glDeleteBuffers(1, &_u_cache);
    glDeleteBuffers(1, &_v_cache);
    glDeleteTextures(1, &_y_texture);
    glDeleteTextures(1, &_u_texture);
    glDeleteTextures(1, &_v_texture);
    glDeleteBuffers(1, &_uv_cache);
#if defined(ENABLE_CUDA_DEVICE)
    cudaGraphicsUnregisterResource(cuda_y_pix_res);
    cudaGraphicsUnregisterResource(cuda_uv_pix_res);
#endif
    glDeleteBuffers(1, &_vertex_bo);
    glDeleteBuffers(1, &_element_bo);
    glDeleteVertexArrays(1, &_vertex_arr_o);

    COMMON_INFO("Renderer deinitialize ok");
}

void VideoViewRenderer::initShader() {
    static const char *vertex_src = "#version 330 core                       \n"
                                    "in vec3 inVertex;                       \n"
                                    "in vec2 inTxtCoord;                     \n"
                                    "out vec2 texCoord;                      \n"
                                    "void main(void)                         \n"
                                    "{                                       \n"
                                    "    gl_Position = vec4(inVertex, 1.0f); \n"
                                    "    texCoord = inTxtCoord;              \n"
                                    "}                                       \n";

    static const char *fragment_src = "#version 330 core                               \n"
                                      "in vec2 texCoord;                               \n"
                                      "uniform int pixelFormat = -1;                   \n"
                                      "uniform sampler2D tex_y;                        \n"
                                      "uniform sampler2D tex_u;                        \n"
                                      "uniform sampler2D tex_v;                        \n"
                                      "uniform sampler2D tex_uv;                       \n"
                                      "void main(void)                                 \n"
                                      "{                                               \n"
                                      "  vec3 yuv;                                     \n"
                                      "  vec3 rgb;                                     \n"
                                      "  if(pixelFormat == 0) {                        \n"
                                      "    yuv.x = texture2D(tex_y, texCoord).r;       \n"
                                      "	   yuv.y = texture2D(tex_u, texCoord).r - 0.5; \n"
                                      "	   yuv.z = texture2D(tex_v, texCoord).r - 0.5; \n"
                                      "  } else if(pixelFormat == 23) {                \n"
                                      "    yuv.x = texture2D(tex_y, texCoord).r;       \n"
                                      "	   yuv.y = texture2D(tex_uv,texCoord).r - 0.5; \n"
                                      "	   yuv.z = texture2D(tex_uv,texCoord).g - 0.5; \n"
                                      "  } else { }                                    \n"
                                      "  rgb = mat3( 1,       1,         1,            \n"
                                      "              0,       -0.3455,   1.779,        \n"
                                      "              1.4075,  -0.7169,   0) * yuv;     \n"
                                      "  gl_FragColor = vec4(rgb, 1.0);                \n"
                                      "}                                               \n";

    _program.addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertex_src);
    _program.addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragment_src);
    _program.bindAttributeLocation("inVertex", 0);
    _program.bindAttributeLocation("inTxtCoord", 1);
    _program.link();
    _program.bind();
}

void VideoViewRenderer::initTexture() {

    //创建像素纹理对象
    glGenBuffers(1, &_y_cache);
    glGenBuffers(1, &_u_cache);
    glGenBuffers(1, &_v_cache);
    glGenBuffers(1, &_uv_cache);
    glGenTextures(1, &_y_texture);
    glGenTextures(1, &_u_texture);
    glGenTextures(1, &_v_texture);
    mVertices << QVector3D(-1, 1, 0.0f) << QVector3D(1, 1, 0.0f) << QVector3D(1, -1, 0.0f)
              << QVector3D(-1, -1, 0.0f);
    mTexcoords << QVector2D(0, 1) << QVector2D(1, 1) << QVector2D(1, 0) << QVector2D(0, 0);
    mVerticesHandle = _program.attributeLocation("inVertex");
    mTexCoordHandle = _program.attributeLocation("inTxtCoord");

    // 前三列点坐标，后两列为纹理坐标
    static GLfloat vertices[] = {
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, //
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f  //
    };
    static GLuint indices[] = {0, 1, 3, 1, 2, 3};

    //使用 glGenVertexArrays 分配顶点数组对象
    glGenVertexArrays(1, &_vertex_arr_o);
    //如果输入的变量array非0，并且是 glGenVertexArrays 所返回的，
    //那么它将创建一个新的顶点数组对象并且与其名称关联起来
    glBindVertexArray(_vertex_arr_o);
    //创建顶点缓存，纹理矩阵
    glGenBuffers(1, &_vertex_bo);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_bo);
    glGenBuffers(1, &_element_bo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_bo);
    // 如果是第一次绑定buffer,且它是一个非零的无符号整型，那么将创建一个与该名称相对应的新缓存对象。
    // 如果绑定到一个已经创建的缓存对象，那么它将成为激活的缓存对象。
    // 如果绑定的buffer值为0，那么OpenGL将不再对当前target应用任何缓存对象
    // GL_STATIC_DRAW 标志标识缓冲区对象数据被修改一次，使用多次，用于绘制
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(mVerticesHandle, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(mVerticesHandle);
    glVertexAttribPointer(mTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          reinterpret_cast<const GLvoid *>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(mTexCoordHandle);
    //用于将当前的顶点属性与顶点缓冲对象（VBO）关联起来
    //输入的变量array为0，那么OpenGL将不再使用程序所分配的任何顶点数组对象，
    //并且将渲染状态重设为顶点数组的默认状态
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);
}

void VideoViewRenderer::synchronize(QQuickFramebufferObject *item) {
    QQuickVideoView *pItem = qobject_cast<QQuickVideoView *>(item);
    if (pItem) {
        if (!_window) {
            _window = pItem->window();
        }
        if (pItem->videoReady()) {
            DecoderController &decoder = pItem->decoder();
            QuickVideoQueue &videoQueue = decoder.videoQueue();
            AVFramePtr image = videoQueue.getRenderFrame();
            if (image != nullptr) {
                if (image->format() == AV_PIX_FMT_YUV420P ||
                    image->format() == AV_PIX_FMT_YUVJ420P) {
                    pixel_format = AV_PIX_FMT_YUV420P;
                    YUV420Texture(image);
                }
#if defined(ENABLE_CUDA_DEVICE)
                if (!image->cpuFrame() &&
                    (image->format() == AV_PIX_FMT_CUDA || image->format() == AV_PIX_FMT_NV12)) {
                    pixel_format = AV_PIX_FMT_NV12;
                    if (enable_device && cu_context == nullptr && cuda_device < 0) {
                        cudaGetDeviceCount(&cuda_device);
                        if (cuda_device > 0) {
                            cudaSetDevice(cuda_device);
                            cudaStreamCreate(&_stream);
                            cuCtxCreate(&cu_context, CU_CTX_SCHED_AUTO, cuda_device);
                        }
                    }
                    NV12Texture(image);
                }
#endif

                /// reset render frame count
                count_renderframe = 0;

                /// 有媒体数据的时候，novideo_status为真
                if (novideo_status) {
                    novideo_status = false;
                    notifyState(false);
                }
            } else {
                /// 是否获得一个nullptr图像指针
                int countnoframe = (count_noframe + 1) * 50;
                if (++count_renderframe % countnoframe == 0) {
                    COMMON_INFO("avframeptr is nullptr, counts {}", count_renderframe);
                    count_noframe = (++count_noframe) % 100;
                    novideo_status = true; ///
                    notifyState(true);
                }
            }
        } else {
            if (++count_renderframe % 50 == 0) {
                if (!novideo_status) { // not ready
                    COMMON_INFO("video not ready");
                    novideo_status = true;
                    notifyState(true);
                }
            }
        }
    }
}

void VideoViewRenderer::notifyState(bool state) {
    // QQuickVideoView* view = const_cast<QQuickVideoView* >(video_viewer);
    // if (view != nullptr) {
    //     view->notifyNoVideoData(state);
    // }
}

void VideoViewRenderer::render() {
    viewRender();

    if (_window) {
        _window->resetOpenGLState();
    }
}

QOpenGLFramebufferObject *VideoViewRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    resize(size.width(), size.height());
    return new QOpenGLFramebufferObject(size, format);
}

void VideoViewRenderer::resize(int width, int height) {
    window_width = width;
    window_height = height;
}

void VideoViewRenderer::autoViewpoint() {
    //// 计算需要显示图片的窗口大小，用于实现长宽等比自适应显示
    if ((double(window_width) / window_height) < (double(image_width) / image_height)) {
        zoomSize.setWidth(window_width);
        zoomSize.setHeight(((double(window_width) / image_width) * image_height));
    } else {
        zoomSize.setHeight(window_height);
        zoomSize.setWidth((double(window_height) / image_height) * image_width);
    }
    mPos.setX(double(window_width - zoomSize.width()) / 2);
    mPos.setY(double(window_height - zoomSize.height()) / 2);
    glViewport(mPos.x(), mPos.y(), zoomSize.width(), zoomSize.height());
}

VideoViewRenderer::VideoViewRenderer(bool device, const QQuickVideoView *callback)
    : QQuickFramebufferObject::Renderer(), enable_device(device), video_viewer(callback) {

    mPos = QPointF(0, 0);
    zoomSize = QSize(0, 0);

    initializeOpenGLFunctions();

    glDepthMask(GL_TRUE);
    glEnable(GL_TEXTURE_2D);

    initShader();
    initTexture();

#if defined(ENABLE_CUDA_DEVICE)
    cuInit(0);
#endif
}

VideoViewRenderer::~VideoViewRenderer() {
    COMMON_INFO("video view control destruction...");
    deinitialize();
}

#if defined(ENABLE_CUDA_DEVICE)
void VideoViewRenderer::cudaCopyPixel2GL(cudaGraphicsResource *_cudaResource, AVFramePtr frame,
                                         int type) {
    void *yv12data = nullptr;
    size_t stride = frame->frame->linesize[0];
    size_t height = frame->frame->height;
    char *dataYuv = new char[stride * height];
    cudaError_t cudaStatus = cudaMalloc((void **) &yv12data, stride * height * 2);
    if (cudaSuccess != cudaStatus) {
        qDebug() << cudaGetErrorString(cudaStatus);
        COMMON_WARN("cuda error {}", cudaGetErrorString(cudaStatus));
        return;
    }

    CUDA_MEMCPY2D cpy;
    memset(&cpy, 0, sizeof(cpy));

    cpy.Height = 100;
    cpy.WidthInBytes = 100;

    cpy.srcPitch = stride;
    cpy.dstPitch = stride;

    cpy.srcMemoryType = CU_MEMORYTYPE_DEVICE;
    cpy.srcDevice = (CUdeviceptr) frame->frame->buf[0]->data;
    cpy.dstMemoryType = CU_MEMORYTYPE_HOST;
    cpy.dstHost = dataYuv;

    CUresult result = cuMemcpy2DAsync(&cpy, _stream);
    cudaStatus = (cudaError_t) result;

    // cudaStatus = cudaMemcpy2D(yv12data, stride, dataYuv,
    //                stride, stride, height, cudaMemcpyHostToDevice);
    if (cudaSuccess != cudaStatus) {
        qDebug() << cudaGetErrorString(cudaStatus);
        COMMON_WARN("cuda error {}", cudaGetErrorString(cudaStatus));
        return;
    }

    delete[] dataYuv;
    cudaStreamSynchronize(_stream);
    cudaFree(yv12data);

    // cudaError_t err = cudaGraphicsMapResources(1, &_cudaResource, nullptr);
    // if (cudaSuccess != err) {
    //     qDebug() << cudaGetErrorString(err);
    //     COMMON_WARN("cuda error {}", cudaGetErrorString(err));
    //     return;
    // }

    // size_t num_bytes = 0;
    // void * data = nullptr;
    // err = cudaGraphicsResourceGetMappedPointer(&data, &num_bytes, _cudaResource);
    // if (cudaSuccess != err) {
    //     qDebug() << cudaGetErrorString(err);
    //     COMMON_WARN("cuda error {}", cudaGetErrorString(err));
    //     cudaGraphicsUnmapResources(1, &_cudaResource, nullptr);
    //     return;
    // }

    // err = cudaMemcpy2D(data, y_linesize, frame, y_linesize, image_width/2, image_height/2,
    // cudaMemcpyDeviceToDevice);
    // //err = cudaMemcpy(data, frame, num_bytes/2, cudaMemcpyDeviceToDevice);
    // if (cudaSuccess != err) {
    //     qDebug() << num_bytes << " " << cudaGetErrorString(err);
    //     COMMON_WARN("cuda error {}", cudaGetErrorString(err));
    //     cudaGraphicsUnmapResources(1, &_cudaResource, nullptr);
    //     return;
    // }

    // err = cudaGraphicsUnmapResources(1, &_cudaResource, nullptr);
    // if (cudaSuccess != err) {
    //     qDebug() << cudaGetErrorString(err);
    //     COMMON_WARN("cuda error {}", cudaGetErrorString(err));
    // }
}
#endif

void VideoViewRenderer::NV12Texture(AVFramePtr frame) {

    if (enable_device) {
#if defined(ENABLE_CUDA_DEVICE)
        if (image_width != frame->imageWidth() || image_height != frame->imageHeight()) {
            y_linesize = frame->linesizeY();
            uv_linesize = frame->linesizeUV();
            image_width = frame->imageWidth();
            image_height = frame->imageHeight();
            if (uv_linesize <= 0) {
                uv_linesize = image_width;
            }

            createGLPixel();
            if (enable_device) {

                cudaError_t err = cudaSuccess;
                if (cudaSuccess != err) {
                    COMMON_WARN("cuda error {}", cudaGetErrorString(err));
                }

                err = cudaGraphicsGLRegisterBuffer(&cuda_y_pix_res, _y_cache,
                                                   cudaGraphicsMapFlagsNone);
                if (cudaSuccess != err) {
                    qDebug() << cudaGetErrorString(err);
                    COMMON_WARN("cuda error {}", cudaGetErrorString(err));
                }

                err = cudaGraphicsGLRegisterBuffer(&cuda_uv_pix_res, _uv_cache,
                                                   cudaGraphicsMapFlagsNone);
                if (cudaSuccess != err) {
                    qDebug() << cudaGetErrorString(err);
                    COMMON_WARN("cuda error {}", cudaGetErrorString(err));
                }
            }
        }

        // gpu memory copy to host
        AVFrame *mframe = av_frame_alloc();
        int rtn = av_hwframe_transfer_data(mframe, frame->frame, 0);

        /// y elements
        // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _y_cache);
        // AVBufferRef *bufref = av_buffer_ref(frame->frame->buf[0]);
        // void * cuda_ptr = av_buffer_get_opaque(bufref);
        // uint8_t * data = bufref->data;
        // size_t size = bufref->size;
        // av_buffer_unref(&bufref);
        cudaCopyPixel2GL(cuda_y_pix_res, frame, 0);
        // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
        /// uv elements
        // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _uv_cache);
        cudaCopyPixel2GL(cuda_uv_pix_res, frame, 1);
        // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);

        av_frame_unref(mframe);
        av_frame_free(&mframe);
#endif
    } else {

        AVFrame *mframe = av_frame_alloc();
        if (av_hwframe_transfer_data(mframe, frame->frame, 0)) {
            if (image_width != mframe->width || image_height != mframe->height) {
                y_linesize = mframe->linesize[0];
                uv_linesize = mframe->linesize[1];
                image_width = mframe->width;
                image_height = mframe->height;
                if (uv_linesize <= 0) {
                    uv_linesize = image_width;
                }
                createGLPixel();
            }
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _y_cache);
            uint8_t *ydata = mframe->data[0];
            glBufferData(GL_PIXEL_UNPACK_BUFFER, y_linesize * image_height, ydata, GL_STREAM_COPY);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _uv_cache);
            uint8_t *uvdata = mframe->data[1];
            glBufferData(GL_PIXEL_UNPACK_BUFFER, uv_linesize * image_height / 2, ydata,
                         GL_STREAM_COPY);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
            av_frame_unref(mframe);
        }

        av_frame_free(&mframe);
    }
}

void VideoViewRenderer::createGLPixel() {

    GLenum docopy = GL_STREAM_DRAW;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _y_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, y_linesize, image_height, 0, GL_RED, GL_UNSIGNED_BYTE,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _y_cache);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, y_linesize * image_height, nullptr, docopy);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);

    if (pixel_format == AV_PIX_FMT_NV12) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _uv_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, uv_linesize / 2, image_height / 2, 0, GL_RED,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _uv_cache);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, uv_linesize / 2 * image_height / 2, nullptr, docopy);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
    } else if (pixel_format == AV_PIX_FMT_YUV420P || pixel_format == AV_PIX_FMT_YUVJ420P) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _u_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, u_linesize, image_height / 2, 0, GL_RED,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _u_cache);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, u_linesize * image_height / 2, nullptr, docopy);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _v_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, v_linesize, image_height / 2, 0, GL_RED,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _v_cache);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, v_linesize * image_height / 2, nullptr, docopy);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
    }
}

void VideoViewRenderer::YUV420Texture(AVFramePtr frame) {

    if (image_width != frame->imageWidth() || image_height != frame->imageHeight()) {

        y_linesize = frame->linesizeY();
        u_linesize = frame->linesizeU();
        v_linesize = frame->linesizeV();

        image_width = frame->imageWidth();
        image_height = frame->imageHeight();

        if (u_linesize <= 0) {
            u_linesize = y_linesize / 2;
        }

        if (v_linesize <= 0) {
            v_linesize = y_linesize / 2;
        }

        createGLPixel();
    }

    /// y elements
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _y_cache);
    uint8_t *ydata = frame->ydata();
    glBufferData(GL_PIXEL_UNPACK_BUFFER, y_linesize * image_height, ydata, GL_STREAM_COPY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
    /// u elements
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _u_cache);
    uint8_t *udata = frame->udata();
    glBufferData(GL_PIXEL_UNPACK_BUFFER, u_linesize * image_height / 2, udata, GL_STREAM_COPY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
    /// v elements
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _v_cache);
    uint8_t *vdata = frame->vdata();
    glBufferData(GL_PIXEL_UNPACK_BUFFER, v_linesize * image_height / 2, vdata, GL_STREAM_COPY);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
}

void VideoViewRenderer::viewRender() {
    if (image_width <= 1 || image_height <= 1) {
        return;
    }

    glDepthMask(TRUE);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // autoViewpoint();
    _program.bind();

    // _program.enableAttributeArray(mVerticesHandle);
    // _program.setAttributeArray(mVerticesHandle, mVertices.constData());
    // _program.enableAttributeArray(mTexCoordHandle);
    // _program.setAttributeArray(mTexCoordHandle, mTexcoords.constData());

    _program.setUniformValue("pixelFormat", pixel_format);

    //用程序对象program作为当前渲染状态的一部分
    //可以使用 glActiveTexture 激活纹理单元
    //绑定这个纹理到当前激活的纹理单元

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _y_texture);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _y_cache);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, y_linesize, image_height, GL_RED, GL_UNSIGNED_BYTE,
                    nullptr);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);

    if (pixel_format == AV_PIX_FMT_NV12) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _uv_texture);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _uv_cache);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, uv_linesize, image_height / 2, GL_RED,
                        GL_UNSIGNED_BYTE, nullptr);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);

        _program.setUniformValue("tex_y", 0);
        _program.setUniformValue("tex_uv", 3);

    } else if (pixel_format == AV_PIX_FMT_YUV420P || pixel_format == AV_PIX_FMT_YUVJ420P) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _u_texture);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _u_cache);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, u_linesize, image_height / 2, GL_RED,
                        GL_UNSIGNED_BYTE, nullptr);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, _v_texture);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _v_cache);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, v_linesize, image_height / 2, GL_RED,
                        GL_UNSIGNED_BYTE, nullptr);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GL_NONE);

        _program.setUniformValue("tex_y", 0);
        _program.setUniformValue("tex_u", 1);
        _program.setUniformValue("tex_v", 2);
    }

    glBindVertexArray(_vertex_arr_o);
    glDrawElements(GL_TRIANGLES,    // 绘制的图元类型
                   6,               // 指定要渲染的元素数(点数)
                   GL_UNSIGNED_INT, // 指定索引中值的类型(indices)
                   nullptr); // 绑定到GL_ELEMENT_ARRAY_BUFFER目标的缓冲区的数组第一个索引偏移量
    glBindVertexArray(0);
    // glDrawArrays(GL_TRIANGLE_FAN, 0, mVertices.size());
    // _program.disableAttributeArray(mVerticesHandle);
    // _program.disableAttributeArray(mTexCoordHandle);
    _program.release();
}
