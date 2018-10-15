
/****************** js2D  *******************
qiangqiang.zhou@united-imaging.com
 
date 20121019
***************************************************/


var enHitType = {};
enHitType.controlPoint0 = 0; // eight controlpoint
enHitType.controlPoint1 = 1;
enHitType.controlPoint2 = 2;
enHitType.controlPoint3 = 3;
enHitType.controlPoint4 = 4;
enHitType.controlPoint5 = 5;
enHitType.controlPoint6 = 6;
enHitType.controlPoint7 = 7;
enHitType.text = 8; // text
enHitType.graph = 9; // graph
enHitType.all = 10; // all (text ,graph .dashedline)
enHitType.None = 11;
enHitType.lineMiddlePoint = 12; //especially for the middle point of line,just for dock not drag.
enHitType.Histogram = 13;
enHitType.Histogram_controlPoint0 = 14;
enHitType.Histogram_controlPoint1 = 15;
enHitType.Histogram_controlPoint2 = 16;
enHitType.Histogram_controlPoint3 = 17;
enHitType.pureText = 18;
enHitType.xline = 19;
enHitType.yline = 20;
var enSizeFitType = {};
enSizeFitType.AutoFit = 0;
enSizeFitType.SizeV1 = 1;
enSizeFitType.SizeV2 = 2;

var enDrawType = {};
enDrawType.line = 0;
enDrawType.arrow = 1;
enDrawType.angle = 2;
enDrawType.rectangle = 3;
enDrawType.circle = 4;
enDrawType.ellipse = 5;
enDrawType.text = 6;
enDrawType.doubleArrow = 7;
enDrawType.point = 8;
enDrawType.R = 9;
enDrawType.L = 10;
enDrawType.maskRect = 11;
enDrawType.maskEllipse = 12;
enDrawType.crossHair = 13;
enDrawType.bound3DBox = 14;


/*
*  enDrawColor
*  for more http://jdstiles.com/colorchart.html and http://jmol.sourceforge.net/jscolors/
*/
var enDrawColor = {};
enDrawColor.black = 'rgba(0, 0, 0, 1.0)';
enDrawColor.blue = 'rgba(0, 0, 255, 1.0)';
enDrawColor.burlywood = 'burlywood';
enDrawColor.crimson = 'crimson ';
enDrawColor.darkcyan = 'darkcyan';
enDrawColor.darkorange = 'darkorange';
enDrawColor.dimgray = 'dimgray';
enDrawColor.firebrick = 'firebrick';
enDrawColor.floralwhite = 'floralwhite';
enDrawColor.green = 'rgba(0, 255, 0, 1.0)';
enDrawColor.indianred = 'indianred';
enDrawColor.orange = 'orange';
enDrawColor.red = 'rgba(255, 0, 0, 1.0)'; //"#FF0000"; //red
enDrawColor.white = 'rgba(255, 255, 255, 1.0)';
enDrawColor.yellow = 'rgba(255, 255, 0, 1.0)';
enDrawColor.selected = 'red';//'rgba(0, 255, 255, 1.0)';
enDrawColor.normal = '#ff8833';//'rgba(59, 192, 0, 1.0)';
enDrawColor.aiblue = '#ff8833';//'rgba(77,136,255,1.0)';//rgba(255, 136, 51, 1)
enDrawColor.aigreen = 'rgba(71,179,107,1.0)';



export function DrawTool() {
    //this.canvasContext = canvasContext;

    //this.text_style = new uih.html.Style();
}

DrawTool.prototype.set_canvas_context = function (canvasContext) {
    this.canvasContext = canvasContext;
};

DrawTool.prototype.test = function () {
    console.log("import succeed");
};

// for line overlay ,include AffiliationText
DrawTool.prototype.drawLine = function (overlay) {
    this.canvasContext.save();
    var x0 = overlay.canvasStart[0];
    var y0 = overlay.canvasStart[1];
    var x1 = overlay.canvasEnd[0];
    var y1 = overlay.canvasEnd[1];
    this.canvasContext.translate(0.5, 0.5);
    this.canvasContext.shadowBlur = 0.5;
    this.canvasContext.shadowOffsetX = 1;
    this.canvasContext.shadowOffsetY = 1;
    this.canvasContext.beginPath();
    this.canvasContext.lineWidth = 1;
    var flag = overlay.is_showControlPoint || overlay.is_selected || overlay.is_grapf_selected;
    if (flag) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = "white";
    }
    this.canvasContext.shadowColor = "black";
    this.canvasContext.moveTo(x0, y0);
    this.canvasContext.lineTo(x1, y1);
    this.canvasContext.stroke();


    //  three lines for one overlay(black  white black)
    //    if (x1!=x0) {
    //        var tan = Math.abs(y1 - y0) / Math.abs(x1 - x0);
    //    }
    //    this.canvasContext.beginPath();
    //    this.canvasContext.lineWidth = 1;
    //    var flag = overlay.is_showControlPoint || overlay.is_selected || overlay.is_grapf_selected;
    //    if (flag) {
    //        this.canvasContext.strokeStyle = overlay.lineColor_selected;
    //    }
    //    else {
    //        this.canvasContext.strokeStyle = "white";
    //    }
    //    if (tan > 1 || x1 == x0) {
    //        this.canvasContext.moveTo(x0 +1 , y0);
    //        this.canvasContext.lineTo(x1+1 , y1);
    //    } else {
    //        this.canvasContext.moveTo(x0, y0 +1);
    //        this.canvasContext.lineTo(x1, y1 + 1);
    //    }
    //    this.canvasContext.stroke();

    //    this.canvasContext.beginPath();
    //    this.canvasContext.strokeStyle = "black";
    //    if (tan > 1 || x1 == x0) {
    //        this.canvasContext.moveTo(x0 + 2, y0);
    //        this.canvasContext.lineTo(x1 + 2, y1);
    //    } else {
    //        this.canvasContext.moveTo(x0, y0 + 2);
    //        this.canvasContext.lineTo(x1, y1 + 2);
    //    }
    //    this.canvasContext.stroke();

    this.canvasContext.restore();
    var lineColor_selected = overlay.lineColor_selected;
    var lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint) {
        this.drawEndPoints(overlay.canvasStart[0], overlay.canvasStart[1], overlay.canvasEnd[0], overlay.canvasEnd[1], lineColor_selected, lineWidth);
    }
};

// just draw a line 
DrawTool.prototype.draw_line = function (startX, startY, endX, endY, lineColor, lineWidth) {

    this.canvasContext.save();
    this.canvasContext.beginPath();
    //this.canvasContext.shadowBlur =1;
    //this.canvasContext.shadowColor = "black";
   
    if (startY - endY == 0) {
        this.canvasContext.moveTo(startX, startY+0.5);
        this.canvasContext.lineTo(endX, endY+0.5);
    }
    if (startX - endX == 0) {
        this.canvasContext.moveTo(startX + 0.5, startY);
        this.canvasContext.lineTo(endX + 0.5, endY);
    }
    this.canvasContext.strokeStyle = lineColor;
    this.canvasContext.lineWidth = lineWidth;
    this.canvasContext.stroke();
    this.canvasContext.restore();
};

DrawTool.prototype.drawCrossHair = function (overlay) {
    if (!gController.getShow3DFlag()) {
        return;
    }
    var centerMargin = 18;
    var lineColorH = overlay.linexColor; //enDrawColor.red;
    var lineColorV = overlay.lineyColor;

    var lineWidth = 1;
    var horStartX = 2;
    var verStartX = overlay.canvasStart[0];
    var horStaryY = overlay.canvasStart[1];
    var horEndX = overlay.canvasWidth - 2;
    var horEndY = horStaryY;

    this.draw_line(horStartX, horStaryY, verStartX - centerMargin , horEndY, lineColorH, lineWidth);
    this.draw_line(verStartX + centerMargin, horStaryY, horEndX, horEndY, lineColorH, lineWidth);
    
    var verStartY = 2;
    var verEndX = verStartX;
    var verEndY = overlay.canvasHeight - 2;
    this.draw_line(verStartX, verStartY, verEndX, horStaryY - centerMargin, lineColorV, lineWidth);
    this.draw_line(verStartX, horStaryY + centerMargin, verEndX, verEndY, lineColorV, lineWidth);
};

DrawTool.prototype.drawPoint = function (overlay) {
    this.canvasContext.save();
    this.canvasContext.translate(0.5, 0.5);
    this.canvasContext.shadowBlur = 0.5;
    this.canvasContext.shadowOffsetX = 1;
    this.canvasContext.shadowOffsetY = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.beginPath();

    this.canvasContext.moveTo(overlay.canvasStart[0] - 5, overlay.canvasStart[1]);
    this.canvasContext.lineTo(overlay.canvasStart[0] + 5, overlay.canvasStart[1]);
    this.canvasContext.moveTo(overlay.canvasStart[0], overlay.canvasStart[1] - 5);
    this.canvasContext.lineTo(overlay.canvasStart[0], overlay.canvasStart[1] + 5);

    if (overlay.is_showControlPoint || overlay.is_selected || overlay.is_grapf_selected) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = overlay.lineColor;
    }
    this.canvasContext.lineWidth = overlay.lineWidth;
    this.canvasContext.stroke();

    this.canvasContext.restore();

};


DrawTool.prototype.drawArrow = function (overlay) {

    var x1 = overlay.canvasStart[0];
    var y1 = overlay.canvasStart[1];
    var x2 = overlay.canvasEnd[0];
    var y2 = overlay.canvasEnd[1];

    var pt1 = [0, 0];
    var pt2 = [0, 0];
    var angle = Math.PI / 2;
    if (x1 == x2 && y1 > y2) {
        angle = -Math.PI / 2;
    }
    if (x1 != x2) {
        angle = Math.atan((y2 - y1) / (x2 - x1));
    }
    var angleLength = 9.0;
    if (x2 < x1) {
        pt1[0] = x1 + angleLength * Math.cos(Math.PI + angle - Math.PI / 6);
        pt1[1] = y1 + angleLength * Math.sin(Math.PI + angle - Math.PI / 6);

        pt2[0] = x1 + angleLength * Math.cos(-Math.PI + angle + Math.PI / 6);
        pt2[1] = y1 + angleLength * Math.sin(-Math.PI + angle + Math.PI / 6);
    }
    else {
        pt1[0] = x1 + angleLength * Math.cos(angle - Math.PI / 6);
        pt1[1] = y1 + angleLength * Math.sin(angle - Math.PI / 6);

        pt2[0] = x1 + angleLength * Math.cos(angle + Math.PI / 6);
        pt2[1] = y1 + angleLength * Math.sin(angle + Math.PI / 6);

    }
    this.canvasContext.lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint || overlay.is_selected || overlay.is_grapf_selected) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = overlay.lineColor;
    }
    this.canvasContext.save();
    this.canvasContext.beginPath();
    this.canvasContext.translate(0.5, 0.5);
    this.canvasContext.shadowBlur = 0.5;

    this.canvasContext.shadowOffsetX = 1;
    this.canvasContext.shadowOffsetY = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.moveTo(x1, y1);
    this.canvasContext.lineTo(x2, y2);
    this.canvasContext.moveTo(pt1[0], pt1[1]);
    this.canvasContext.lineTo(x1, y1);
    this.canvasContext.lineTo(pt2[0], pt2[1]);
    this.canvasContext.stroke();
    this.canvasContext.restore();
    var lineColor_selected = overlay.lineColor_selected;
    if (overlay.is_showControlPoint) {
        this.drawEndPoints(overlay.canvasStart[0], overlay.canvasStart[1], overlay.canvasEnd[0], overlay.canvasEnd[1], lineColor_selected, overlay.lineWidth);
    }

};

DrawTool.prototype.drawDoubleArrow = function (overlay) {
    var x1 = overlay.canvasStart[0];
    var y1 = overlay.canvasStart[1];
    var x2 = overlay.canvasEnd[0];
    var y2 = overlay.canvasEnd[1];
    var pt1 = [0, 0];
    var pt2 = [0, 0];
    var pt3 = [0, 0];
    var pt4 = [0, 0];
    var angle = 90.0;
    if (x1 != x2) {
        angle = Math.atan((y2 - y1) / (x2 - x1));
    }
    //draw arrow
    var angleLength = 9.0;
    if (x2 < x1) {
        pt1[0] = x1 + angleLength * Math.cos(Math.PI + angle - Math.PI / 6);
        pt1[1] = y1 + angleLength * Math.sin(Math.PI + angle - Math.PI / 6);

        pt2[0] = x1 + angleLength * Math.cos(-Math.PI + angle + Math.PI / 6);
        pt2[1] = y1 + angleLength * Math.sin(-Math.PI + angle + Math.PI / 6);

        pt3[0] = x2 + angleLength * Math.cos(angle - Math.PI / 6);
        pt3[1] = y2 + angleLength * Math.sin(angle - Math.PI / 6);

        pt4[0] = x2 + angleLength * Math.cos(angle + Math.PI / 6);
        pt4[1] = y2 + angleLength * Math.sin(angle + Math.PI / 6);
    }
    else {
        pt1[0] = x1 + angleLength * Math.cos(angle - Math.PI / 6);
        pt1[1] = y1 + angleLength * Math.sin(angle - Math.PI / 6);

        pt2[0] = x1 + angleLength * Math.cos(angle + Math.PI / 6);
        pt2[1] = y1 + angleLength * Math.sin(angle + Math.PI / 6);

        pt3[0] = x2 + angleLength * Math.cos(Math.PI + angle - Math.PI / 6);
        pt3[1] = y2 + angleLength * Math.sin(Math.PI + angle - Math.PI / 6);

        pt4[0] = x2 + angleLength * Math.cos(-Math.PI + angle + Math.PI / 6);
        pt4[1] = y2 + angleLength * Math.sin(-Math.PI + angle + Math.PI / 6);
    }
    this.canvasContext.lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint || overlay.is_selected || overlay.is_grapf_selected) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
        this.canvasContext.fillStyle = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = overlay.lineColor;
        this.canvasContext.fillStyle = overlay.lineColor;
    }
    this.canvasContext.save();
    this.canvasContext.shadowBlur = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.beginPath();
    this.canvasContext.moveTo(x1, y1);
    this.canvasContext.lineTo(pt1[0], pt1[1]);
    this.canvasContext.lineTo(pt2[0], pt2[1]);
    this.canvasContext.fill();
    this.canvasContext.beginPath();
    this.canvasContext.moveTo((pt1[0] + pt2[0]) / 2, (pt1[1] + pt2[1]) / 2);
    this.canvasContext.lineTo((pt3[0] + pt4[0]) / 2, (pt3[1] + pt4[1]) / 2);
    this.canvasContext.stroke();
    this.canvasContext.beginPath();
    this.canvasContext.moveTo(x2, y2);
    this.canvasContext.lineTo(pt3[0], pt3[1]);
    this.canvasContext.lineTo(pt4[0], pt4[1]);
    this.canvasContext.fill();
    this.canvasContext.restore();

//    var isTextSelected = false;
//    if (overlay.is_text_selected || overlay.is_selected) { isTextSelected = true; }
//    this.drawAffiliationText(overlay, pa);

    var lineColor_selected = overlay.lineColor_selected;
    var lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint) {
        this.drawEndPoints(overlay.canvasStart[0], overlay.canvasStart[1], overlay.canvasEnd[0], overlay.canvasEnd[1], lineColor_selected, lineWidth);
    }

    var dashedLineColor = overlay.is_dash_selected ? overlay.lineColor_selected : enDrawColor.burlywood;
    if (!overlay.is_text_fixed)
        this.drawDashedLine(overlay.affiliationTextOverlay.dashedLineCanvasStart, overlay.affiliationTextOverlay.dashedLineCanvasEnd, [6, 4],
                                       dashedLineColor, lineWidth);
};

DrawTool.prototype.drawAngle = function (overlay) {

    var sx = overlay.canvasStart[0];
    var sy = overlay.canvasStart[1];
    var cx = overlay.canvasCenter[0];
    var cy = overlay.canvasCenter[1];
    var ex = overlay.canvasEnd[0];
    var ey = overlay.canvasEnd[1];

    this.canvasContext.save();
    this.canvasContext.shadowBlur = 0.5;
    this.canvasContext.shadowOffsetX = 1;
    this.canvasContext.shadowOffsetY = 1;
    this.canvasContext.translate(0.5, 0.5);
    this.canvasContext.shadowColor = "black";
    this.canvasContext.beginPath();
    this.canvasContext.moveTo(sx, sy);
    this.canvasContext.lineTo(cx, cy);
    this.canvasContext.moveTo(sx, sy);
    this.canvasContext.lineTo(ex, ey);

    this.canvasContext.lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint || overlay.is_grapf_selected) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = overlay.lineColor;
    }

    origin = [sx, sy];
    var p1 = [cx, cy];
    var p2 = [ex, ey];
    var clockwise = GetClockDirection(origin, p1, p2);

    // Get the between angle
    var angle = GetDegree(origin, p1, p2);
    angle = DegreeToRadian(angle);

    //Get startAngle on the basis of canvasContext.arc coordinate system
    var startAngle;
    var temp = (cy - sy) / (cx - sx);
    if ((cy - sy) <= 0) {
        if (temp < 0) {
            temp = Math.abs(temp);
            startAngle = Math.PI * 2 - Math.atan(temp);
        }
        else {
            startAngle = Math.PI + Math.atan(temp);
        }
    }
    else {
        if (temp < 0) {
            temp = Math.abs(temp);
            startAngle = Math.PI - Math.atan(temp);
        }
        else {
            startAngle = Math.atan(temp);
        }
    }
    var dis1 = GetDistance(overlay.canvasStart, overlay.canvasCenter);
    var dis2 = GetDistance(overlay.canvasStart, overlay.canvasEnd);
    var radius = Math.min(dis1, dis2) / 10;
    if (radius > 30) radius = 30;
    this.canvasContext.moveTo(sx, sy);
    if (clockwise) {
        this.canvasContext.arc(sx, sy, radius, startAngle, startAngle + angle, false);
    }
    else {
        this.canvasContext.arc(sx, sy, radius, startAngle, startAngle - angle, true);
    }
    this.canvasContext.stroke();
    this.canvasContext.restore();
    var lineColor_selected = overlay.lineColor_selected;
    var lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint) {
        this.drawRectPoint(overlay.canvasStart[0], overlay.canvasStart[1], 2.5, lineColor_selected, lineWidth);
        this.drawRectPoint(overlay.canvasCenter[0], overlay.canvasCenter[1], 2.5, lineColor_selected, lineWidth);
        this.drawRectPoint(overlay.canvasEnd[0], overlay.canvasEnd[1], 2.5, lineColor_selected, lineWidth);
    }

};



DrawTool.prototype.drawRect = function (overlay) {

    var x1 = Math.round(overlay.canvasStart[0]);
    var y1 = Math.round(overlay.canvasStart[1]);
    var x2 = Math.round(overlay.canvasEnd[0]);
    var y2 = Math.round(overlay.canvasEnd[1]);

    this.canvasContext.lineWidth = 1;
    if (overlay.is_showControlPoint || overlay.is_grapf_selected) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = overlay.getNormalColor();//enDrawColor.normal;//'green';//"white";
        //set color
    }
    this.canvasContext.save();
    this.canvasContext.translate(0.5, 0.5);
    this.canvasContext.shadowBlur = 0.5;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.shadowOffsetX = 1;
    this.canvasContext.shadowOffsetY = 1;
    this.canvasContext.beginPath();
    this.canvasContext.strokeRect(x1, y1, x2 - x1, y2 - y1);
    this.canvasContext.restore();

    var lineColor_selected = overlay.lineColor_selected;
    var lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint) {
        this.drawControlPoints(overlay.canvasStart[0], overlay.canvasStart[1], overlay.canvasEnd[0], overlay.canvasEnd[1], lineColor_selected, lineWidth);
    }

};

DrawTool.prototype.drawRectangle = function (p, width, height) {
    this.canvasContext.strokeStyle = enDrawColor.selected;
    this.canvasContext.save();
    this.canvasContext.shadowBlur = 0.5;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.beginPath();
    this.canvasContext.strokeRect(p[0]-1, p[1], width + 2, height+3);
    this.canvasContext.restore();
};

DrawTool.prototype.drawEllipse = function (overlay) {
    this.canvasContext.lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint || overlay.is_selected || overlay.is_grapf_selected) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = overlay.lineColor;
    }
    var result = (overlay.canvasEnd[1] - overlay.canvasStart[1]) / (overlay.canvasEnd[0] - overlay.canvasStart[0]);
    var radians = Math.atan(result);
    var cx = (overlay.canvasEnd[0] + overlay.canvasStart[0]) / 2;
    var cy = (overlay.canvasEnd[1] + overlay.canvasStart[1]) / 2;
    var oa = GetDistance(overlay.canvasEnd, overlay.canvasStart) / 2;
    var ob, x, y;
    if (overlay.keepAxis == 0) {
        ob = oa * 0.618;
    } else {
        ob = overlay.keepAxis * overlay.canvasWidth;
    }
    var k = (oa / 0.75);
    this.canvasContext.save();
    this.canvasContext.shadowBlur = 0.5;
    this.canvasContext.shadowOffsetX = 1;
    this.canvasContext.shadowOffsetY = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.translate(cx, cy);
    this.canvasContext.rotate(radians);
    this.canvasContext.beginPath();
    this.canvasContext.moveTo(0, -ob);
    this.canvasContext.bezierCurveTo(k, -ob, k, ob, 0, ob);
    this.canvasContext.bezierCurveTo(-k, ob, -k, -ob, 0, -ob);
    this.canvasContext.stroke();
    this.canvasContext.restore();

    var lineColor_selected = overlay.lineColor_selected;
    var lineWidth = overlay.lineWidth;
    if (overlay.is_showControlPoint) {
        this.drawRectPoint(overlay.controlPoint0[0], overlay.controlPoint0[1], 2.5, lineColor_selected, lineWidth);
        this.drawRectPoint(overlay.controlPoint1[0], overlay.controlPoint1[1], 2.5, lineColor_selected, lineWidth);
        this.drawRectPoint(overlay.controlPoint2[0], overlay.controlPoint2[1], 2.5, lineColor_selected, lineWidth);
        this.drawRectPoint(overlay.controlPoint3[0], overlay.controlPoint3[1], 2.5, lineColor_selected, lineWidth);
    }
};


DrawTool.prototype.drawDot = function (x, y, color, size) {
    this.canvasContext.save();
    this.canvasContext.shadowBlur = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.beginPath();

    this.canvasContext.moveTo(x, y);
    this.canvasContext.lineTo(x + size, y + size);
    this.canvasContext.strokeStyle = color;
    this.canvasContext.lineWidth = 1;
    this.canvasContext.stroke();

    this.canvasContext.restore();
};

DrawTool.prototype.dottedLine = function (x1, y1, x2, y2, interval, color) {
    this.canvasContext.save();
    this.canvasContext.shadowBlur = 1;
    this.canvasContext.shadowColor = "black";
    if (!interval) {
        interval = 5;
    }
    var isHorizontal = true;
    if (x1 == x2) {
        isHorizontal = false;
    }

    var len = isHorizontal ? x2 - x1 : y2 - y1;
    this.canvasContext.beginPath();
    this.canvasContext.moveTo(x1, y1);
    this.canvasContext.arc(x1, y1, 1, 0, 2 * Math.PI, true);
    var progress = 0;
    while (len > progress) {
        progress += interval;
        if (progress > len) {
            progress = len;
        }

        if (isHorizontal) {
            this.canvasContext.moveTo(x1 + progress, y1);
            this.canvasContext.arc(x1 + progress, y1, 1, 0, 2 * Math.PI, true);
        } else {
            this.canvasContext.moveTo(x1, y1 + progress);
            this.canvasContext.arc(x1, y1 + progress, 1, 0, 2 * Math.PI, true);
        }
    }
    this.canvasContext.closePath();

    this.canvasContext.fill();

    this.canvasContext.lineWidth = 1;
    this.canvasContext.strokeStyle = color;
    this.canvasContext.stroke();

    this.canvasContext.restore();
};

DrawTool.prototype.drawRectDot = function (x1, y1, x2, y2, color, size, text) {
    this.dottedLine(x1, y1, x2, y1, 10, color);
    this.dottedLine(x1, y2, x2, y2, 10, color);
    this.dottedLine(x1, y1, x1, y2, 10, color);
    this.dottedLine(x2, y1, x2, y2, 10, color);
};

DrawTool.prototype.drawControlPoints = function (x1, y1, x2, y2, color, size) {
    var rad = 2.5;
    var xMiddle = (x1 + x2) / 2;
    var yMiddle = (y1 + y2) / 2;

    this.drawRectPoint(x1, y1, rad, color, size);
    this.drawRectPoint(x2, y2, rad, color, size);
    this.drawRectPoint(x1, y2, rad, color, size);
    this.drawRectPoint(x2, y1, rad, color, size);
    this.drawRectPoint(xMiddle, y1, rad, color, size);
    this.drawRectPoint(x1, yMiddle, rad, color, size);
    this.drawRectPoint(x2, yMiddle, rad, color, size);
    this.drawRectPoint(xMiddle, y2, rad, color, size);
};

DrawTool.prototype.drawEndPoints = function (x1, y1, x2, y2, color, size) {
    var rad = 2.5;
    this.drawRectPoint(x1, y1, rad, color, size);
    this.drawRectPoint(x2, y2, rad, color, size);
};

/**
* @Draw circle command.
* @param circle The circle to draw.
* @param app The application to draw the circle on.
*/
DrawTool.prototype.drawCircle = function (centerX, centerY, radius, color, size, bfill) {
    this.canvasContext.save();

    this.canvasContext.beginPath();
    this.canvasContext.arc(centerX, centerY, radius, 0, 2 * Math.PI);
    this.canvasContext.closePath();

    if (bfill) {
        this.canvasContext.fillStyle = enDrawColor.blue;
        this.canvasContext.fill();
    }

    this.canvasContext.lineWidth = 1;
    this.canvasContext.strokeStyle = color;
    this.canvasContext.stroke();

    this.canvasContext.restore();
};
DrawTool.prototype.drawRectPoint = function (centerX, centerY, radius, color, size) {
    this.canvasContext.save();

    this.canvasContext.beginPath();
    this.canvasContext.lineWidth = size;
    this.canvasContext.strokeStyle = color;
    this.canvasContext.strokeRect(centerX - radius, centerY - radius, 2 * radius, 2 * radius);
    this.canvasContext.restore();
};
DrawTool.prototype.drawQuadraticCurve = function (sx, sy, cx, cy, ex, ey, lineColor, lineWidth) {
    this.canvasContext.save();

    this.canvasContext.beginPath();
    this.canvasContext.moveTo(sx, sy);
    this.canvasContext.quadraticCurveTo(cx, cy, ex, ey);
    this.canvasContext.closePath();

    this.canvasContext.strokeStyle = lineColor;
    this.canvasContext.lineWidth = lineWidth;
    this.canvasContext.stroke();

    this.canvasContext.restore();
};

DrawTool.prototype.drawDashedLine = function (start, end, da, lineColor, lineWidth) {
    if (start == [0, 0] && end == [0, 0]) {
        return;
    }
    if (!da)
        da = [10, 5];
    this.canvasContext.save();
    this.canvasContext.shadowBlur = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.beginPath();
    var dx = (end[0] - start[0]), dy = (end[1] - start[1]);
    var len = Math.sqrt(dx * dx + dy * dy);
    var rot = Math.atan2(dy, dx);
    this.canvasContext.translate(start[0], start[1]);
    this.canvasContext.moveTo(0, 0);
    this.canvasContext.rotate(rot);
    var dc = da.length;
    var di = 0, draw = true;
    x = 0;
    while (len > x) {
        x += da[di++ % dc];
        if (x > len) x = len;
        draw ? this.canvasContext.lineTo(x, 0) : this.canvasContext.moveTo(x, 0);
        draw = !draw;
    }
    this.canvasContext.closePath();

    this.canvasContext.strokeStyle = "white";
    this.canvasContext.lineWidth = 1;
    this.canvasContext.stroke();

    this.canvasContext.restore();
};

/*
*  drawPlus
*/
DrawTool.prototype.drawPlus = function (point) {
    this.drawLine(point[0] - 5, point[1], point[0] + 5, point[1], enDrawColor.red, 1);
    this.drawLine(point[0], point[1] - 5, point[0], point[1] + 5, enDrawColor.red, 1);
};

/*
x,y     
a,b         
color      
size        
precision   
**********************************/
DrawTool.prototype.drawOval = function (x, y, a, b, color, size, precision) {
    var i;
    var iMax = 2 * Math.PI;
    var step = 2 * Math.PI / (precision * Math.sqrt(a * b) * 4.5);
    for (i = 0; i < iMax; i += step) {
        this.drawDot(x + a * Math.cos(i), y + b * Math.sin(i), color, size);
    }
};

/**************************
x,y    
r     
n       
color 
size   
style   
=0  
=1   
=2 
**********************************/
DrawTool.prototype.drawPoly = function (x, y, r, n, color, size) {
    var i;
    var theta = Math.PI;
    var x1 = x, y1 = y - r, x2, y2;
    for (i = 0; i < n; i++) {
        theta -= (2 * Math.PI / n);
        x2 = x + r * Math.sin(theta);
        y2 = y + r * Math.cos(theta);

        this.drawLine(x1, y1, x2, y2, color, size);

        x1 = x2;
        y1 = y2; //alert(x1+" "+y1)
    }
};

// Point
//
function Point(x, y) {
    this.x = x;
    this.y = y;
}
Point.compareInt = function (x, y) {
    return x < y ? -1 : x > y ? 1 : 0;
};
Point.compare = function (p1, p2) {
    if (p1.x == p2.x) return Point.compareInt(p1.y, p2.y);
    return Point.compareInt(p1.x, p2.x);
};
Point.prototype.eq = function (p2) {
    return Point.compare(this, p2) == 0;
};
Point.prototype.lt = function (p2) {
    return Point.compare(this, p2) < 0;
};
Point.prototype.le = function (p2) {
    return Point.compare(this, p2) <= 0;
};
Point.prototype.gt = function (p2) {
    return Point.compare(this, p2) > 0;
};
Point.prototype.ge = function (p2) {
    return Point.compare(this, p2) >= 0;
};

// Slope
//
function Slope(p1, p2) {
    var rise = p1.y - p2.y;
    var run = p1.x - p2.x;
    var gcd = Slope.gcd(Math.abs(rise), Math.abs(run));
    rise /= gcd;
    run /= gcd;
    if (run < 0) {
        rise *= -1;
        run *= -1;
    } else if (run == 0) {
        rise = 1;
    }
    this.rise = rise;
    this.run = run;
}
Slope.prototype.eq = function (s2) {
    return this.rise == s2.rise && this.run == s2.run;
};
Slope.gcd = function (x, y) {
    if (y == 0) return x;
    return Slope.gcd(y, x % y);
};

// Line
//
function Line(p1, p2) {
    if (Point.compare(p1, p2) > 0) {
        this.p1 = p2;
        this.p2 = p1;
    } else {
        this.p1 = p1;
        this.p2 = p2;
    }
    this.slope = new Slope(this.p1, this.p2);
}
Line.prototype.overlaps = function (line, includeEndPt) {
    if (!this.slope.eq(line.slope)) return false;
    if (!this.p1.eq(line.p1) &&
      !this.slope.eq(new Line(this.p1, line.p1).slope)) return false;
    if (this.p1.lt(line.p2) && this.p2.gt(line.p1)) return true;
    if (includeEndPt && this.p1.le(line.p2) && this.p2.ge(line.p1)) return true;
    return false;
};
Line.prototype.erase = function (line) {
    var lines = [];
    if (this.p1.ge(line.p1) && this.p2.le(line.p2)) return lines;
    if (this.p1.ge(line.p1)) {
        lines.push(new Line(line.p2, this.p2));
    } else if (this.p2.le(line.p2)) {
        lines.push(new Line(this.p1, line.p1));
    } else {
        lines.push(new Line(this.p1, line.p1));
        lines.push(new Line(line.p2, this.p2));
    }
    return lines;
};
Line.prototype.merge = function (line) {
    var p1 = this.p1.lt(line.p1) ? this.p1 : line.p1;
    var p2 = this.p2.gt(line.p2) ? this.p2 : line.p2;
    return new Line(p1, p2);
};

DrawTool.prototype.drawTextBorder = function(textW, textH, textLocation, lineColor, lineWidth) {
    var x = textLocation[0];
    var y = textLocation[1];
    this.canvasContext.save();
    this.canvasContext.lineWidth = lineWidth;
    this.canvasContext.strokeStyle = lineColor;
    this.canvasContext.beginPath();
    this.canvasContext.strokeRect(x, y, textW, textH);
    this.canvasContext.restore();
};
//Histogram
//
DrawTool.prototype.drawHistogram = function (overlay) {

    var affiliationHistogramOverlay = overlay.affiliationHistogramOverlay;
    if (!affiliationHistogramOverlay.histogram) return;
    var histogram = affiliationHistogramOverlay.histogram;
    //   overlay.setHistogramControlPoint(overlay.canvasStart, overlay.canvasEnd, painter);

    var lineColor;
    if (overlay.affiliationHistogramOverlay.is_selected || overlay.is_selected) {
        this.canvasContext.strokeStyle = overlay.lineColor_selected;
        lineColor = overlay.lineColor_selected;
    }
    else {
        this.canvasContext.strokeStyle = overlay.lineColor;
        lineColor = overlay.lineColor;
    }
    var lineWidth = affiliationHistogramOverlay.lineWidth;
    var intensity_range = histogram["intensity_range"];
    var histogram_max = histogram["histogram_max"];
    var histogramData = histogram["data"];
    var histogramW = overlay.affiliationHistogramOverlay.canvasControlPoint2[0] - overlay.affiliationHistogramOverlay.canvasControlPoint1[0];
    var step = histogramW / histogramData.length;
    var minx = overlay.affiliationHistogramOverlay.canvasControlPoint1[0];
    var maxx = overlay.affiliationHistogramOverlay.canvasControlPoint2[0];
    var maxy = overlay.affiliationHistogramOverlay.canvasControlPoint1[1];
    var miny = overlay.affiliationHistogramOverlay.canvasControlPoint0[1];
    var dis = maxy - miny;
    this.canvasContext.save();
    this.canvasContext.beginPath();
    this.canvasContext.lineWidth = 1;
    this.canvasContext.shadowBlur = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.moveTo(minx, maxy);
    // use three lines to draw every rectangle 
    for (var i = 0; i < histogramData.length; i++) {
        var x1 = minx + i * step;
        var y1 = maxy - histogramData[i] * dis;
        var x2 = x1 + step;
        var y2 = maxy;
        this.canvasContext.lineTo(x1, y1);
        this.canvasContext.lineTo(x2, y1);
        this.canvasContext.lineTo(x2, y2);
    }
    this.canvasContext.closePath();
    this.canvasContext.stroke();
    this.canvasContext.restore();

    var line_length = 5;
    this.draw_line(minx, miny, minx, maxy, lineColor, lineWidth);
    //   this.draw_line(minx, maxy, maxx, maxy, lineColor, lineWidth);
    this.draw_line(minx, miny, minx - line_length, miny, lineColor, lineWidth);
    this.draw_line(minx, Math.round((miny + maxy) / 2), minx - line_length, Math.round((miny + maxy) / 2), lineColor, lineWidth);
    this.draw_line(minx, maxy, minx - line_length, maxy, lineColor, lineWidth);
    this.draw_line(minx, maxy, minx, maxy + line_length, lineColor, lineWidth);
    this.draw_line(Math.round((minx + maxx) / 2), maxy, Math.round((minx + maxx) / 2), maxy + line_length, lineColor, lineWidth);
    this.draw_line(maxx, maxy, maxx, maxy + line_length, lineColor, lineWidth);
    this.drawLabelOfAixs("" + histogram_max, minx, miny, 'y');
    this.drawLabelOfAixs("" + histogram_max / 2, minx, (miny + maxy) / 2, 'y');
    this.drawLabelOfAixs("" + 0, minx, maxy, 'y');
    this.drawLabelOfAixs("" + intensity_range[0], minx, maxy, 'x');
    this.drawLabelOfAixs("" + (intensity_range[0] + intensity_range[1]) / 2, (minx + maxx) / 2, maxy, 'x');
    this.drawLabelOfAixs("" + intensity_range[1], maxx, maxy, 'x');
    var linePosition = overlay.getHistogramDashedLine(overlay.canvasStart, overlay.canvasEnd);
    var dashedStart = [linePosition[0], linePosition[1]];
    var dashedEnd = [linePosition[2], linePosition[3]];
    this.drawDashedLine(dashedStart, dashedEnd, [6, 4], enDrawColor.burlywood, lineWidth);
    if (affiliationHistogramOverlay.is_selected) {
        var rad = 2.5;
        this.drawRectPoint(minx, miny, rad, lineColor, lineWidth);
        this.drawRectPoint(minx, maxy, rad, lineColor, lineWidth);
        this.drawRectPoint(maxx, maxy, rad, lineColor, lineWidth);
        //      this.drawRectPoint(maxx, miny, rad, lineColor, lineWidth);
    }
};
DrawTool.prototype.drawLabelOfAixs = function (text, x, y, axis) {
    var spacing = 5;
    var text_width = this.measureText(text);
    var text_height = getTextHeight(this.canvasContext.font).height;
    if (axis == 'y') {
        this.drawTextImpl(text, x - text_width - spacing, y - text_height / 2);
    }
    else if (axis == 'x') {
        this.drawTextImpl(text, x - text_width / 2, y + spacing);
    }

}


DrawTool.prototype.drawText = function (text, x, y, w, h, fontColor, fontSize, lineColor, lineWidth) {
    if (text == undefined || text == "")
        return;

    this.drawTextImpl(text, x, y, fontColor, fontSize);
};


DrawTool.prototype.drawAffiliationText = function (overlay, painter, isPureText) {
    if (!overlay) { return; }
    var text;
    var textLocation;
    var isRL = false;
    if (isPureText) {
        text = overlay.text;
        textLocation = overlay.canvasStart;
    } else {
        text = overlay.affiliationTextOverlay.text;
        if (overlay.getTextLocation) textLocation = overlay.getTextLocation(painter);
    }
    if (!text) return;
    if (overlay.style == enDrawType.L || overlay.style == enDrawType.R) {
        isRL = true;
    }
    this.drawTextImpl(text, textLocation[0], textLocation[1], isRL);
    if (overlay.is_text_selected || (overlay.style == enDrawType.text &&overlay.is_selected)) {
        this.drawRectangle(textLocation, overlay.textW, overlay.textH);
    }
};



DrawTool.prototype.measureText = function (text) {

    this.canvasContext.save();
    this.canvasContext.textBaseline = "Top";
    var maxWidth = 1;
    var lines = text.split("\n");
    for (var i = 0; i < lines.length; i++) {
        var width = this.canvasContext.measureText(lines[i]).width;
        if (width > maxWidth)
            maxWidth = width;
    }
    this.canvasContext.restore();
    return maxWidth;
};




DrawTool.prototype.drawTextImpl = function (text, x, y, isRL) {
    this.canvasContext.save();
    this.canvasContext.textBaseline = "Top";
    this.canvasContext.fillStyle = enDrawColor.white;
    if (isRL) {
        var font_size = this.canvasContext.font.split(" ")[0].split("px")[0];
        var size = font_size - '0' + 2;
        this.canvasContext.font = "bold " + (size) + "px Arial";
    }
    var lines = text.split("\n");
    var textH = getTextHeight(this.canvasContext.font).height;
    for (var i = 0; i < lines.length; i++) {
        this.canvasContext.fillText(lines[i], x, y + (i + 1) * textH);
        setShadow(this.canvasContext, lines[i],x, y + (i + 1) * textH);
    }

    

    this.canvasContext.restore();
};


DrawTool.prototype.drawDashLine = function (start, end, center, delta, xx) {
    var len = Math.max(Math.abs(end[1] - start[1]), Math.abs(end[0] - start[0]));
    var draw = true;
    var x = 0;

    this.canvasContext.save();
    this.canvasContext.beginPath();
    this.canvasContext.moveTo(start[0], start[1]);
    var dis = 0;
    //white
    while (len > x) {
        x += delta;

        if (xx) {
            dis = Math.abs(x + start[0] - center[0]);
            draw && (dis > 2 * delta) ? this.canvasContext.lineTo(x + start[0], start[1]) : this.canvasContext.moveTo(x + start[0], start[1]);
        } else {
            dis = Math.abs(start[1] + x - center[1]);
            draw && (dis > 2 * delta) ? this.canvasContext.lineTo(start[0], start[1] + x) : this.canvasContext.moveTo(start[0], start[1] + x);
        }

        draw = !draw;
    }

    this.canvasContext.closePath();

    this.canvasContext.strokeStyle = enDrawColor.white;
    this.canvasContext.lineWidth = 2;
    this.canvasContext.stroke();
    this.canvasContext.restore();

    ///
    this.canvasContext.save();
    this.canvasContext.beginPath();
    if (xx) {
        this.canvasContext.moveTo(start[0] + delta, start[1]);
    } else {
        this.canvasContext.moveTo(start[0], start[1] + delta);
    }

    draw = true;
    x = delta;
    //black
    while (len > x) {
        x += delta;
        if (xx) {
            dis = Math.abs(x + start[0] - center[0]);
            draw && (dis > 2 * delta) ? this.canvasContext.lineTo(x + start[0], start[1]) : this.canvasContext.moveTo(x + start[0], start[1]);
        } else {
            dis = Math.abs(start[1] + x - center[1]);
            draw && (dis > 2 * delta) ? this.canvasContext.lineTo(start[0], start[1] + x) : this.canvasContext.moveTo(start[0], start[1] + x);
        }

        draw = !draw;
    }

    this.canvasContext.closePath();

    this.canvasContext.strokeStyle = enDrawColor.black;
    this.canvasContext.lineWidth = 2;
    this.canvasContext.stroke();
    this.canvasContext.restore();
}
/*
*drawCrossLine
*/
DrawTool.prototype.drawCrossLine = function (centerPt) {
    var da = 4;
    var len = centerPt[0] / 10;
    var start = [centerPt[0] - len, centerPt[1]];
    var end = [centerPt[0] + len, centerPt[1]];
    this.drawDashLine(start, end, centerPt, da, true);

    start = [centerPt[0], centerPt[1] - len];
    end = [centerPt[0], centerPt[1] + len];
    this.drawDashLine(start, end, centerPt, da, false);
};


DrawTool.prototype.drawEllipseMask = function (overlay) {
    var result = (overlay.canvasEnd[1] - overlay.canvasStart[1]) / (overlay.canvasEnd[0] - overlay.canvasStart[0]);
    var radians = Math.atan(result);
    var cx = (overlay.canvasEnd[0] + overlay.canvasStart[0]) / 2;
    var cy = (overlay.canvasEnd[1] + overlay.canvasStart[1]) / 2;
    var oa = GetDistance(overlay.canvasEnd, overlay.canvasStart) / 2;
    var ob;
    if (overlay.keepAxis == 0) {
        ob = oa * 0.618;
    } else {
        ob = overlay.keepAxis * overlay.canvasWidth;
    }
    var k = (oa / 0.75);
 //   this.canvasContext.restore();
    this.canvasContext.save();
    this.canvasContext.strokeStyle = overlay.lineColor_selected;
    this.canvasContext.shadowBlur = 1;
    this.canvasContext.shadowColor = "black";
    this.canvasContext.translate(cx, cy);
    this.canvasContext.rotate(radians);
    this.canvasContext.beginPath();
    this.canvasContext.moveTo(0, -ob);
    this.canvasContext.bezierCurveTo(k, -ob, k, ob, 0, ob);
    this.canvasContext.bezierCurveTo(-k, ob, -k, -ob, 0, -ob);
    this.canvasContext.stroke();
    this.canvasContext.setTransform(1, 0, 0, 1, 0, 0);
};


DrawTool.prototype.drawRectMask = function (overlayItem) {

    var startX = overlayItem.canvasStart[0];
    var startY = overlayItem.canvasStart[1];
    var endX = overlayItem.canvasEnd[0];
    var endY = overlayItem.canvasEnd[1];
//    this.canvasContext.restore();
    this.canvasContext.save();
    this.canvasContext.beginPath();
    this.canvasContext.rect(startX, startY, endX - startX, endY - startY);
    this.canvasContext.strokeStyle = enDrawColor.selected;
    this.canvasContext.stroke();

};

DrawTool.prototype.drawClip = function (overlayItem) {

    var canvasWidth = overlayItem.canvasWidth;
    var canvasHeight = overlayItem.canvasHeight;
    this.canvasContext.fillStyle = 'rgba(0,0,0,1)';
    this.canvasContext.fillRect(0, 0, canvasWidth, canvasHeight);
    this.canvasContext.clip();
    this.canvasContext.clearRect(0, 0, canvasWidth, canvasHeight);
    this.canvasContext.restore();
};
DrawTool.prototype.drawItem = function (drawType, overlayItem) {
    switch (drawType) {
        case enDrawType.line:
            this.drawLine(overlayItem);
            break;
        case enDrawType.arrow:
            this.drawArrow(overlayItem);
            break;
        case enDrawType.rectangle:
            this.drawRect(overlayItem);
            break;
        case enDrawType.circle:
            break;
        case enDrawType.ellipse:
            this.drawEllipse(overlayItem);
            break;
        case enDrawType.text:
            break;
        case enDrawType.point:
            this.drawPoint(overlayItem);
            break;
        case enDrawType.maskRect:
            this.drawRectMask(overlayItem);
            break;
        case enDrawType.maskEllipse:
            this.drawEllipseMask(overlayItem);
            break;
        case enDrawType.crossHair:
            this.drawCrossHair(overlayItem);
            break;
        case enDrawType.bound3DBox:
            this.drawRect(overlayItem);
            break;
        default:
            break;
    }

};

var cc = new DrawTool();


