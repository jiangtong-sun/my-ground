// 导入Qt Quick核心模块，最低支持版本2.15，提供QML基础UI元素与声明式语法支持
import QtQuick 2.15
// 导入Qt Quick窗口模块，最低支持版本2.15，提供窗口相关的基础功能（此处未直接创建窗口，为后续扩展预留）
import QtQuick.Window 2.15
// 导入Qt Quick控件模块，最低支持版本2.15，提供按钮、文本框等基础交互控件
import QtQuick.Controls 2.15
// 导入Qt Quick布局模块，最低支持版本2.15，提供网格、列、行等结构化布局能力
import QtQuick.Layouts 1.15

// 导入地面控制站自定义QML模块（GcsQuick），版本1.0，包含项目专属的UI组件与工具（如GcsTextButton）
import GcsQuick 1.0

/**
 * 根容器组件：飞行数据帧元素网格展示容器
 * 功能：作为地面站数据展示的载体，封装网格布局的数据展示逻辑，承载多个飞行参数按钮组件
 * 说明：Item是QML的基础无视觉样式容器，仅用于布局与属性封装，适合作为复杂组件的根节点
 */
Item {
    id: itemNode    // 组件唯一标识，用于内部属性、子组件的引用与关联
    // 自定义属性：控件预设宽度，用于配置整个容器的基础宽度（可外部传入修改）
    property int ctlWidth: 100
    // 自定义属性：控件预设高度，用于配置整个容器的基础高度（可外部传入修改）
    property int ctlHeight: 100
    // 自定义属性：当前网络通道编号，绑定全局命令管理器（commandMgr）的网络通道属性，用于数据通信通道选择
    property int currChl: commandMgr.netChannel
    // 自定义属性：数据元素总数，优先从数据帧模型（dataFrameModel）的列表模型中获取总数，模型不存在时默认50个
    property int countElem: dataFrameModel?dataFrameModel.listModel.count:50
    // 自定义属性：网格单元格宽度步长，根据数据总数动态计算（≥50个元素分5列，否则分4列），保证布局紧凑美观
    property real stepW: countElem>=50?itemNode.width/5:itemNode.width/4
    // 自定义属性：网格单元格高度步长，固定按14等分计算容器高度，保证垂直方向布局均匀
    property real stepH: itemNode.height/14

    // 绑定容器高度与预设高度一致，确保布局尺寸可控
    height: ctlHeight
    // 绑定容器宽度与预设宽度一致，确保布局尺寸可控
    width: ctlWidth
 
    /**
    * 网格视图组件：用于批量展示数据帧中的所有参数项
    * 功能：以网格形式自动排列数据模型中的所有元素，实现批量数据的结构化展示，支持滚动（默认）、裁剪
    * 类比：像Excel表格一样，按行按列自动填充数据内容，每个单元格对应一个飞行参数
    */
    GridView {
        // 开启裁剪功能，超出GridView可视区域的内容将被隐藏，避免界面布局混乱
        clip: true
        // 填充父容器（itemNode），充分利用容器的可用空间，实现自适应布局
        anchors.fill: parent
        // 网格单元格宽度，绑定之前计算的宽度步长，保证单元格大小均匀
        cellWidth: stepW
        // 网格单元格高度，绑定之前计算的高度步长，保证单元格大小均匀
        cellHeight: stepH
        // 网格数据模型：绑定数据帧模型的列表模型，模型不存在时设为null（容错处理）
        // 作用：为GridView提供数据源，每一条模型数据对应一个网格单元格的内容
        model: dataFrameModel?dataFrameModel.listModel:null

        /**
        * 网格委托组件：定义单个单元格的UI样式与数据绑定逻辑
        * 功能：为GridView的每一条模型数据生成对应的UI展示单元，此处使用自定义按钮（GcsTextButton）展示飞行参数
        * 说明：delegate是QML列表/网格组件的核心，实现"一份模板，批量复用"
        */
        delegate: GcsTextButton {
            // 自定义属性：存储当前委托对应的模型数据对象（飞行参数单条数据）
            property var fectElem: object
            // 自定义属性：提取飞行参数的单位（如%、m、km/h），用于后续界面展示
            property string fectUnits: fectElem.units

            /**
            * 组件完成加载回调：委托组件创建并初始化完成后执行
            * 功能：初始化当前飞行参数的展示内容与颜色，触发数据手动更新，完成初始状态渲染
            */
            Component.onCompleted:  {
                // 容错判断：确保数据对象有效且包含display（展示值）属性，避免空指针异常
                if (fectElem && fectElem.display) {
                    // 处理展示值：若颜色标识大于0xf（15），显示"--"（表示数据无效/未获取），否则显示模型中的展示值
                    var show = (fectElem.showColor > 0xf) ? qsTr("--") : fectElem.display
                    // 绑定文本颜色：通过颜色映射表（txtColorMap）获取对应颜色，&0xf取低4位（过滤高位冗余数据，仅保留颜色标识）
                    oneElem.textColor = txtColorMap[fectElem.showColor & 0xf]
                    // 拼接展示文本：将参数值与单位组合（如"50%"、"100m"），展示在按钮上
                    oneElem.textValue = show + fectUnits
                    // 触发手动更新：调用数据对象的更新方法，传入当前网络通道，获取最新的飞行参数数据
                    fectElem.manualUpdateData(currChl)
                }
            }

            /**
            * 信号连接组件：监听数据对象（fectElem）的DisplayChanged信号
            * 功能：实现"数据变化→UI实时刷新"，当飞行参数数据更新时，自动同步更新界面展示
            */
            Connections {
                // 信号监听目标：绑定当前委托的数据对象（fectElem），即监听该对象发出的信号
                target: fectElem

                /**
                * 数据展示变化回调：监听fectElem的DisplayChanged信号，接收更新后的参数值与颜色标识
                * @param value  更新后的飞行参数展示值
                * @param icolor 更新后的颜色标识（用于异常告警视觉提示）
                */
                function onDisplayChanged(value, icolor) {
                    // 处理展示值：若颜色标识大于0xf（15），显示"--"，否则显示更新后的参数值
                    var show = (icolor > 0xf ? qsTr("--"): value)
                    // 更新文本颜色：通过颜色映射表获取对应颜色，&0xf取低4位，保留有效颜色标识
                    oneElem.textColor = txtColorMap[icolor & 0xf]
                    // 更新展示文本：拼接参数值与单位，实现UI实时同步刷新
                    oneElem.textValue = show + fectUnits
                }
            }
            id: oneElem     // 单个按钮组件的唯一标识，用于内部属性引用
            height: stepH   // 按钮高度绑定网格单元格高度，保证布局对齐
            width: stepW    // 按钮宽度绑定网格单元格宽度，保证布局对齐
            btnFontSize: TextStyle.BodySmall    // 按钮字体大小，使用全局文本样式的小号字体，保证可读性
            labelValue: fectElem.name           // 按钮标签文本，绑定飞行参数的名称（如"电池电量"、"经度"）
        }
    }
}
