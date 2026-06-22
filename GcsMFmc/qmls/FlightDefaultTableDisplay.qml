import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import GcsQuick 1.0

/**
 * GCS地面控制站参数分组展示根组件
 * 核心功能：按自定义分组展示飞行/设备参数，支持动态配置布局尺寸、分组名称，实时刷新参数值与告警颜色
 * 设计特点：嵌套Repeater+Grid实现“分组-参数项”的双层动态布局，适配不同数量的分组/参数项
 */
Item {
    id: root
    //var：QML 的通用动态类型，可存储任意数据（对象、数组、字符串、数字、null 等）
    property var dataModePart: null
    property var subGroupStr: null
    property int navItemHeight: 36
    property int navItemWidth: 180

    /**
    * 布局配置初始化函数（外部调用入口）
    * 功能：动态设置参数分组的布局尺寸、数据源、分组名称，适配不同数量的分组/参数项
    * @param statusMode  参数数据源（各分组的参数列表）
    * @param eachHeight  单个参数项的高度（覆盖默认navItemHeight）
    * @param groupStr    分组名称列表（覆盖默认subGroupStr）
    */
    function setModePart(statusMode, eachHeight, groupStr) {
        // 获取分组数量，容错处理：若分组为空则默认1个（避免布局崩溃）
        var count = groupStr.length
        count = count > 0? count: 1;
        dataModePart = statusMode
        subGroupStr = groupStr
        navItemHeight = eachHeight
        // 动态计算单个分组的宽度：根组件宽度均分至每个分组（减2px留边距）
        navItemWidth = (root.width-2)/count
        // 宽度上限限制：最大300px（避免分组过宽导致界面失衡）
        navItemWidth = navItemWidth>300?300:navItemWidth
    }

    /**
    * 外层行布局：横向排列所有参数分组
    * 功能：将多个参数分组按水平方向排列，实现“多列分组”的整体布局
    */
    Row{
        // 填充根组件，充分利用可用空间
        anchors.fill: parent

        /**
        * 外层重复器：为每个参数分组生成对应的展示容器
        * 模型：dataModePart（参数分组数据源），每一条模型数据对应一个参数分组
        */
        Repeater{
            model: dataModePart

            /**
            * 分组容器委托：定义单个参数分组的UI结构（标题+分割线+参数网格）
            * 功能：为每个分组生成独立的展示区域，包含分组标题、装饰分割线、参数项网格
            */
            delegate: Item {
                // 自定义属性：当前分组的参数数据源（单个分组的所有参数项）
                property var dataMode: object
                // 自定义属性：当前分组的参数项总数
                property var devFirstCount: dataMode.count
                // 自定义属性：参数网格的列数（向上取整，按剩余高度均分参数项，保证布局紧凑）
                // 计算逻辑：剩余高度 = 父容器高度 - 标题栏高度(navItemHeight)，每列高度=navItemHeight，列数=总参数数/每行可放的数量
                property int columnFirst: Math.ceil(devFirstCount / ((secondHeight - navItemHeight)/navItemHeight))
                // 自定义属性：单个参数项的宽度（列数均分分组宽度，减20px留边距，保证列间距均匀）
                property int firstItemWidth: (columnFirst * navItemWidth - 20)/columnFirst
                // 自定义属性：分组标题左右分割线的宽度（分组宽度的1/4，保证视觉对称）
                property int firstLROffset: (columnFirst * navItemWidth)/4

                // 分组容器宽度：列数*单个参数项宽度（适配参数网格的总宽度）
                width: columnFirst * navItemWidth
                // 分组容器高度：继承父容器（Row）高度
                height: parent.height

                /**
                * 左侧装饰分割线（GCS自定义组件）
                * 功能：分组标题左侧的视觉装饰，提升界面层次感
                */
                GcsDivider{
                    orientation: Qt.Horizontal  // 水平分割线
                    anchors.top: parent.top     // 对齐分组容器顶部
                    anchors.left: parent.left   // 对齐分组容器左侧
                    anchors.leftMargin: 10      // 左侧边距10px
                    width: firstLROffset        // 宽度=预计算的偏移量
                    height: navItemHeight       // 高度=标题栏高度
                    spacing: 20                 // 分割线内部间距（装饰用）
                }

                /**
                * 右侧装饰分割线（GCS自定义组件）
                * 功能：分组标题右侧的视觉装饰，与左侧分割线对称，强化分组标题区域
                */
                GcsDivider{
                    orientation: Qt.Horizontal  // 水平分割线
                    anchors.top: parent.top     // 对齐分组容器顶部
                    anchors.right: parent.right // 对齐分组容器右侧
                    anchors.rightMargin: 10     // 右侧边距10px
                    width: firstLROffset        // 宽度=预计算的偏移量
                    height: navItemHeight       // 高度=标题栏高度
                    spacing: 20                 // 分割线内部间距（装饰用）
                }

                /**
                * 分组标题文本（GCS自定义文本组件）
                * 功能：展示当前分组的名称，居中显示在分割线之间
                */
                GcsText {
                    height: navItemHeight                   // 高度=标题栏高度
                    anchors.top: parent.top                 // 对齐分组容器顶部
                    anchors.right: parent.right             // 对齐分组容器右侧
                    anchors.left: parent.left               // 对齐分组容器左侧
                    anchors.leftMargin: firstLROffset       // 左侧边距=分割线宽度（避开分割线）
                    anchors.rightMargin: firstLROffset      // 右侧边距=分割线宽度（避开分割线）
                    horizontalAlignment: Text.AlignHCenter  // 水平居中
                    verticalAlignment: Text.AlignVCenter    // 垂直居中
                    text: subGroupStr[index]                // 绑定当前分组的名称（index为Repeater的当前项索引）
                }

                /**
                * 参数网格容器：承载当前分组的所有参数项
                * 功能：在分组标题下方，生成网格布局展示所有参数项，适配参数数量动态调整行列
                */
                Item {
                    anchors.top: parent.top             // 对齐分组容器顶部
                    anchors.bottom: parent.bottom       // 对齐分组容器底部
                    anchors.topMargin: navItemHeight    // 顶部边距=标题栏高度（避开标题）
                    anchors.left: parent.left           // 对齐分组容器左侧
                    anchors.leftMargin: 10              // 左侧边距10px
                    anchors.right: parent.right         // 对齐分组容器右侧
                    anchors.rightMargin: 10             // 右侧边距10px

                    /**
                    * 网格布局：按计算的列数排列参数项
                    * 功能：自动将参数项按列数分行排列，实现参数的结构化展示
                    */
                    Grid{
                        anchors.fill: parent    // 填充父容器（参数网格容器）
                        columns: columnFirst    // 列数=预计算的columnFirst（动态适配参数数量）
                        spacing: 1              // 单元格间距1px（紧凑布局）

                        /**
                        * 内层重复器：为当前分组的每个参数项生成按钮组件
                        * 模型：dataMode（当前分组的参数列表），每一条模型数据对应一个参数项
                        */
                        Repeater {
                            model: dataMode

                            /**
                            * 参数项委托：GCS自定义文本按钮组件
                            * 功能：展示单个参数的名称、值，支持实时刷新值和告警颜色
                            */
                            delegate: GcsTextButton {
                                // 自定义属性：当前参数项的数据源（包含名称、值、颜色等信息）
                                property var displayNode: object

                                id: dataelem                    // 参数项按钮唯一标识
                                width: firstItemWidth           // 宽度=预计算的单个参数项宽度
                                height: navItemHeight           // 高度=单个参数项高度
                                labelValue: displayNode.itemName// 按钮标签=参数名称

                                /**
                                * 组件初始化回调：参数项按钮创建完成后执行
                                * 功能：初始化参数值和颜色，容错处理无效数据
                                */
                                Component.onCompleted:  {
                                    // 容错：确保参数数据源有效且包含展示值
                                    if (displayNode && displayNode.display) {
                                        // 数据有效性判断：颜色标识>0xf（15）则显示"--"（无效/未获取），否则显示参数值
                                        dataelem.textValue = ((displayNode.showColor > 0xf)) ? qsTr("--") : displayNode.display
                                        // 绑定告警颜色：通过颜色映射表（txtColorMap）获取，&0xf取低4位（过滤冗余数据）
                                        dataelem.textColor = txtColorMap[displayNode.showColor & 0xf]
                                    }
                                }

                                /**
                                * 信号连接：监听参数数据源的更新信号
                                * 功能：参数值/颜色变化时，实时刷新UI展示（数据驱动UI）
                                * 类比：给参数项装了“实时通知器”，数据一变立刻更新界面
                                */
                                Connections{
                                    target: displayNode // 监听目标=当前参数项的数据源

                                    /**
                                    * 参数更新回调：接收新值和新颜色标识
                                    * @param valueStr  更新后的参数展示值
                                    * @param iColor    更新后的告警颜色标识
                                    */
                                    function onUpdateColorValue(valueStr, iColor){
                                        // 数据有效性判断：颜色标识>0xf则显示"--"，否则显示新值
                                        dataelem.textValue = (iColor > 0xf) ? qsTr("--") : valueStr;
                                        // 更新告警颜色：&0xff取低8位（兼容更多颜色标识）
                                        dataelem.textColor =  txtColorMap[icolor & 0xff]
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
