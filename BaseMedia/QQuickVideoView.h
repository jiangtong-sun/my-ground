#pragma once

#include "DecoderController.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QQuickFramebufferObject>
#include <QTimerEvent>
#include <QTimer>

#if defined(ENABLE_CUDA_DEVICE)
#include <cuda/cuda.h>
#include <cuda/cuda_gl_interop.h>
#endif

class QQuickVideoView;
class VideoViewRenderer : public QQuickFramebufferObject::Renderer,
                          protected QOpenGLFunctions_3_2_Core {

#if defined(ENABLE_CUDA_DEVICE)
    cudaStream_t _stream = nullptr;
    CUcontext cu_context = nullptr;
    cudaGraphicsResource *cuda_y_pix_res = nullptr;
    cudaGraphicsResource *cuda_uv_pix_res = nullptr;
#endif

    bool enable_device = false;
    int cuda_device = -1;

    QSizeF zoomSize;
    QPointF mPos;

    QMatrix4x4 mProjection;

    QOpenGLShaderProgram _program;
    QVector<QVector3D> mVertices;
    QVector<QVector2D> mTexcoords;

    GLuint _vertex_bo = 0;    //顶点缓冲对象
    GLuint _vertex_arr_o = 0; //顶点数组对象
    GLuint _element_bo = 0;   //元素缓冲对象

    GLuint _y_texture = 0;
    GLuint _u_texture = 0;
    GLuint _v_texture = 0;
    GLuint _uv_texture = 0;
    GLuint _y_cache = 0;
    GLuint _u_cache = 0;
    GLuint _v_cache = 0;
    GLuint _uv_cache = 0;


    AVFramePtr dataFrame = nullptr;
    QQuickWindow *_window = nullptr;
    const QQuickVideoView* video_viewer = nullptr;
    bool novideo_status = false;
    int count_renderframe = 0;
    int count_noframe = 0;
    int mVerticesHandle = 0;
    int mTexCoordHandle = 0;
    int window_width = 0;
    int window_height = 0;
    int pixel_format = 0;
    int y_linesize;
    int u_linesize;
    int v_linesize;
    int uv_linesize;
    int image_width;
    int image_height;

public:
    VideoViewRenderer(bool device, const QQuickVideoView* callback);
    ~VideoViewRenderer();

    void synchronize(QQuickFramebufferObject *item) override;
    void render() override;
    void notifyState(bool);

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

protected:
    void viewRender();
    void initTexture();
    void initShader();
    void deinitialize();
    void autoViewpoint();

    void resize(int width, int height);
    void YUV420Texture(AVFramePtr frame);
    void NV12Texture(AVFramePtr frame);
    void createGLPixel();

#if defined(ENABLE_CUDA_DEVICE)
    void cudaCopyPixel2GL(cudaGraphicsResource *res, AVFramePtr frame, int type);
#endif
};

class BASEMEDIA_EXPORT QQuickVideoView : public QQuickFramebufferObject {

    Q_OBJECT
    MEDIA_PROPERTY_AUTO(bool, enableDevice)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

  public:
    QQuickVideoView(QQuickItem *parent = nullptr);
    ~QQuickVideoView();

    Renderer *createRenderer() const override;

    DecoderController &decoder() { return controller; }

    bool videoReady() { return _videoReady; }

    QString url() { return _url; }

    void notifyVideoState(const bool);

    void setUrl(const QString &url);

    Q_INVOKABLE void replayStream(bool cpu, bool enable);
    Q_INVOKABLE void openStream(bool);
    Q_INVOKABLE void closeStream();

    Q_INVOKABLE void setRWTimeout(int);

    static void registerType();

  private:
    void timerEvent(QTimerEvent *event) override;

  public slots:
    void scheduleReconnect();
    void onVideoInfoReady(int width, int height);

  signals:
    void urlChanged();
    void vidoeStateChanged(bool);
    void videoMessage(float fps, float KBps, QString err);

  private:
    QString _url;
    int _updateTimer = 0;
    int _videoparam = 0;
    QTimer reconn_timer;
    std::atomic_bool _videoReady;
    std::atomic_int _videoHeight;
    std::atomic_int _videoWidth;
    DecoderController controller;
};
