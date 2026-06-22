import QtQuick 2.15
import QtQuick.Controls 2.15
import GcsQuick 1.0

Rectangle {//分别编辑3个分量，回车有效，空格放弃
    id: aglEdtr_
    height: txDeg_.height
    width:300
    color: "transparent"
    border.width: 1
    border.color: bdcolor//txcolor
    property int n_deg: 0
    property int n_min: 0
    property real f_sec: 0.0
    property real f_deg: 0.0
    property real f_sgn: 1.0//符号，负表示西经南纬。在国内可不使用
    property int fntsize: TextStyle.Body
    property string fntfmly: TextStyle.family
    property bool islongitude: true//经度或纬度，影响取值范围
    property string txcolor: ""
    property string bdcolor: ""
    property int leftmrgn: 0

    function deg2dms(deg){//小数转度分秒
        f_sgn=1.0
        if(deg<0.0){
            f_sgn=-1
            deg=deg*-1.0
        }
        n_deg=Math.trunc(deg)
        deg=(deg-n_deg)*60.0
        n_deg=n_deg*f_sgn
        n_min=Math.trunc(deg)
        f_sec=(deg-n_min)*60.0
        //同步到分量的显示内容
        edDeg_.text=String(n_deg)
        if(f_sgn<0.0&&n_deg===0){
            edDeg_.text="-"+edDeg_.text
        }
        edMin_.text=String(n_min)
        edSec_.text=f_sec.toFixed(3)//精度在3cm内
    }

    function dms2deg(){//度分秒转小数
        var f_deg
        f_deg=n_deg+n_min/60.0+f_sec/3600.0
        f_deg*=f_sgn
        return f_deg
    }

    function adjLeftMargin(){
        var contentLen=edDeg_.width+txDeg_.width+ edMin_.width+txMin_.width+ edSec_.width+txSec_.width
        var margin=width-contentLen
        if(margin>1){
            edDeg_.anchors.leftMargin=margin/2
        }
    }

    signal endEdit(bool valid)

    TextField{
        id:edDeg_
        width:54
        enabled: true
        font.pixelSize: fntsize
        font.family: fntfmly
        palette {
                base: aglEdtr_.color  // 背景色
                text: txcolor // 文字颜色
            }
        background: Rectangle {// 消除选中边框
            color: aglEdtr_.color
            border.width: 0
        }

        anchors.left: parent.left
        anchors.leftMargin: leftmrgn
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        validator: RegExpValidator{
            regExp: islongitude?/^-?(0|[1-9]?[0-9]|[1]?[0-7]?[0-9])$/:/^-?(0|[1-8]?[0-9])$/
        }
        onTextEdited: {
            n_deg=Number(edDeg_.text)
            if(n_deg<0||edDeg_.text.charAt(0)==='-'){
                n_deg=n_deg*-1
                f_sgn=-1.0
            }
            else{
                f_sgn=1.0
            }
        }
        Keys.onReturnPressed: {
            endEdit(true)//编辑有效
        }
        Keys.onSpacePressed: {
            endEdit(false)//放弃编辑
        }
    }
    Text {
        id: txDeg_
        text: "°"
        anchors.left: edDeg_.right
        anchors.top: parent.top
        width: 8
        color: txcolor
    }

    TextField{
        id:edMin_
        width:40
        font.pixelSize: fntsize
        font.family: fntfmly
        palette {
                base: aglEdtr_.color  // 背景色
                text: txcolor // 文字颜色
            }
        background: Rectangle {
            color: aglEdtr_.color
            border.width: 0
        }
        anchors.left: txDeg_.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        validator: RegExpValidator{
            regExp: /^-?(0|[1-5]?[0-9])$/
        }

        onTextEdited: {
            n_min=Number(edMin_.text)
        }
        Keys.onReturnPressed: {
            endEdit(true)//编辑有效
        }
        Keys.onSpacePressed: {
            endEdit(false)//放弃编辑
        }
    }
    Text {
        id: txMin_
        text: "\'"
        anchors.left: edMin_.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 10
        color: txcolor
    }

    TextField{
        id:edSec_
        width:90
        font.pixelSize: fntsize
        font.family: fntfmly
        palette {
                base: aglEdtr_.color  // 背景色
                text: txcolor // 文字颜色
            }
        background: Rectangle {
            color: aglEdtr_.color
            border.width: 0
        }
        anchors.left: txMin_.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        validator: RegExpValidator {
            regExp: /^\d{1,2}(\.\d{1,4})?$/
        }
        onTextEdited: {
            f_sec=Number(edSec_.text)
        }
        Keys.onReturnPressed: {
            endEdit(true)//编辑有效
        }
        Keys.onSpacePressed: {
            endEdit(false)//放弃编辑
        }
    }
    Text {
        id: txSec_
        text: "\""
        anchors.left: edSec_.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 10
        color: txcolor
    }
}
