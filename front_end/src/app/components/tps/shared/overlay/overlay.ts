import { extend } from "webdriver-js-extender";
import { OnInit } from '@angular/core';
import { Point } from '../tools/point'
declare var createjs: any;

export class Overlay extends createjs.Shape {
    type: string;
    overlayStage: any;
    protected _cps: Array<any>;
    protected _startPoint: Point;
    protected _endPoint: Point;
    protected _tempPoint: Point;

    constructor(stage, type) {
        super();
        this.overlayStage = stage;
        this.type = type;
        this.addEventListener("mousedown", this.handleMouseDown.bind(this));
        this.addEventListener("pressmove", this.handlePressMove.bind(this));
        this.addEventListener("dblclick", this.handleDbClick.bind(this));
        this.addEventListener("pressup", this.handlePressUp.bind(this));
        this.addEventListener("mouseover", this.handleMouseOver.bind(this));
        this.overlayStage.addChild(this);
    }

    setStartPoint(p) {
        this._startPoint = p;
    }

    setEndPoint(p) {
        this._endPoint = p;
    }

    updateCps(delta_x: number, delta_y: number) {
        // if (typeof(this._cp_coords) == 'undefined') {
        //     return;
        // }
        // this._cp_coords.forEach(cp => {
        //     cp.x += delta_x;
        //     cp.y += delta_y;
        // });
    }

    drawControlPoints() {
        if (typeof(this._cps) == null) {
            return;
        }
        this._cps.forEach(cp => {
            cp.update()
        });
    };

    update(){
    }

    handleMouseDown(evt) {
        console.log('handleMouseDown')
        this._tempPoint = new Point(0, 0);
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
    }
    handlePressMove(evt) {
        let delta_x = evt.stageX - this._tempPoint.x;
        let delta_y = evt.stageY - this._tempPoint.y;
        this._tempPoint.x = evt.stageX;
        this._tempPoint.y = evt.stageY;
        
        // update start point and end point
        evt.currentTarget.x += delta_x;
        evt.currentTarget.y += delta_y;
        evt.currentTarget.updateCps(delta_x, delta_y);

        this.update();
    }
    handlePressUp(evt) {
        console.log('handlePressUp')
        this._tempPoint = new Point(0, 0);
    }
    handleDbClick(evt) {
        console.log('handleDbClick')
        this.curTarget = evt.currentTarget;
        this.overlayStage.removeChild(this.curTarget);
        this.overlayStage.clear();
        this.overlayStage.update();
    }
    handleMouseOver(evt) {
        console.log('handleMouseOver on', evt.currentTarget.type)
    }
}

// /**
//  * Dapeng/Filming共用
//  * 此文件提供了 直线 图元的绘制及方法
//  */

// import { OnInit } from '@angular/core';


// export class Line implements OnInit {
//     cstart: Point;      // c represent canvas
//     cend: Point;
//     istart: Point;      // i represent image
//     iend: Point;
//     isFixed: boolean;
//     textArr: Array<string>;
//     text_cstart: Point;
//     text_istart: Point;
//     text_width = 0;
//     text_height = 0;
//     cp: Array<Point>;                   // control points
//     hitIndex: number;
//     fontsize = 12;
//     isShowOverlayInfo = true;

//     ngOnInit() {

//     }

//     clone = function (line) {
//         this.istart = line.istart;
//         this.iend = line.iend;
//         this.hitIndex = line.hitIndex;
//         this.type = line.type;

//         this.isFixed = line.isFixed;
//         this.textArr = line.textArr;
//         this.text_istart = line.text_istart;
//         this.text_width = line.text_width;
//         this.text_height = line.text_height;
//         this.isUpdateTextPosition = true;
//     };

//     set_start(cpos) {
//         this.cstart = cpos;
//         // this.istart = unproject(this.cstart);
//     };

//     set_end(cpos) {
//         this.cend = cpos;
//         // this.iend = unproject(this.cend);
//     };

//     set_istart(ipos) {
//         this.istart = ipos;
//         // this.cstart = app.anti_unproject(this.istart);
//     };

//     set_iend(ipos) {
//         this.iend = ipos;
//         // this.cend = app.anti_unproject(this.iend);
//     };

//     // imageToCanvas(app, image) {
//     //     this.cstart = app.anti_unproject(this.istart);
//     //     this.cend = app.anti_unproject(this.iend);
//     //     this.setControlPoint();
//     //     this.setTextPosition(app, image);
//     // };

//     update(mouseStartX, mouseStartY, mouseEndX, mouseEndY, app, image) {
//         var i = this.hitIndex;
//         switch (i) {
//             // case 0:
//             //     var startPoint = this.calculatePointOnBorderForOverlay(mouseStartX, mouseStartY, mouseEndX, mouseEndY, this.cstart.x, this.cstart.y, app);
//             //     this.cstart.x = startPoint[0];
//             //     this.cstart.y = startPoint[1];
//             //     break;
//             // case 1:
//             //     var endPoint = this.calculatePointOnBorderForOverlay(mouseStartX, mouseStartY, mouseEndX, mouseEndY, this.cend.x, this.cend.y, app);
//             //     this.cend.x = endPoint[0];
//             //     this.cend.y = endPoint[1];
//             //     break;
//             // case 2:       // hit all
//             //     var diff = this.canculateMoveDistanceForOverlay(mouseStartX, mouseStartY, mouseEndX, mouseEndY, app);
//             //     var xdiff = diff[0];
//             //     var ydiff = diff[1];
//             //     this.cstart.x += xdiff;
//             //     this.cstart.y += ydiff;
//             //     this.cend.x += xdiff;
//             //     this.cend.y += ydiff;
//             //     break;
//             case 3:
//                 var textStartPoint = this.calculatePointOnBorderForOverlay(mouseStartX, mouseStartY, mouseEndX, mouseEndY, this.text_cstart.x, this.text_cstart.y, app, [null, null, image.width - this.text_width, image.height - this.text_height]);
//                 this.text_cstart.x = textStartPoint[0];
//                 this.text_cstart.y = textStartPoint[1];
//                 this.isFixed = false;
//                 break;
//             default:
//         }
//         this.istart = app.unproject(this.cstart);
//         this.iend = app.unproject(this.cend);
//         this.text_istart = app.unproject(this.text_cstart);

//         this.setControlPoint();
//         // this.setText(app, image);
//         // this.setTextPosition(app, image);
//     };

//     setControlPoint() {
//         this.cp[0] = new Point(Math.round(this.cstart.x), Math.round(this.cstart.y));
//         this.cp[1] = new Point(Math.round(this.cend.x), Math.round(this.cend.y));
//     };

//     isHited(cpos) {
//         this.hitIndex = -1;
//         var len = this.cp.length;
//         if (len == 0) return false;
//         for (var i = 0; i < len; ++i) {
//             // if (dist(this.cp[i], cpos) < 25) {
//             //     this.hitIndex = i;
//             //     return true;
//             // }
//         }
//         // if (this.disPointToLine(cpos, this.cp[0], this.cp[1]) < 5) {
//         //     this.hitIndex = 2;
//         //     return true;
//         // }

//         if (this.isHitText(cpos)) {
//             this.hitIndex = 3;
//             return true;
//         }
//         return false;
//     };

//     setTextPosition(app, image) {
//         // if (this.isFixed) {
//         //     var pos = uih.dwv.uti.GetMaxRightPoint([this.cstart, this.cend]);
//         //     var x = pos.x + 8;
//         //     var y = pos.y - this.text_height / 2;

//         //     if (x + this.text_width > image.width) x = image.width - this.text_width - 8;
//         //     if (x <= 0) x = 0;
//         //     if (y <= 0) y = 0;
//         //     if (y >= image.height - this.text_height) y = image.height - this.text_height;

//         //     this.text_cstart = new Point(x, y);
//         //     this.text_istart = app.unproject(this.text_cstart);
//         // } else {
//         //     if (image.isSizeChanged || this.isUpdateTextPosition) {
//         //         this.text_cstart = app.anti_unproject(this.text_istart);
//         //         var x = this.text_cstart.x;
//         //         var y = this.text_cstart.y;
//         //         if (x <= 0) x = 0;
//         //         if (x + this.text_width > image.width) x = image.width - this.text_width;
//         //         if (y <= 0) y = 0;
//         //         if (y >= image.height - this.text_height) y = image.height - this.text_height;
//         //         this.text_cstart = new Point(x, y);
//         //         this.isUpdateTextPosition = false;
//         //     }
//         //     //图像大小不发生变化，但发生翻转等影响转换矩阵的操作时，也需修改ip的值
//         //     this.text_istart = app.unproject(this.text_cstart);
//         //     var position = uih.dwv.uti.GetDashPosition([this.cstart, this.cend], this.text_cstart, this.text_height, this.text_width);
//         //     this.dash_cstart = position[0];
//         //     this.dash_cend = position[1];
//         // }
//     };

//     setText() {
//         // let length = 0;
//         // var text = "*";
//         // var minY = Math.floor(Math.min(this.istart.y, this.iend.y));
//         // var maxY = Math.floor(Math.max(this.istart.y, this.iend.y));
//         // var minX = Math.floor(Math.min(this.istart.x, this.iend.x));
//         // var maxX = Math.floor(Math.max(this.istart.x, this.iend.x));
//         // if (minY < 0 || maxY > app.file.Rows
//         //     || minX < 0 || maxX > app.file.Columns) {  //Rows 图像的高  Columns 图像的宽
//         //     length = 0;
//         //     text = "Length: " + length + "Pixel";
//         // } else {
//         //     length = this.distance(this.istart, this.iend);
//         //     if (app.file.PixelSpacing
//         //         && app.file.PixelSpacing.length == 2
//         //         && app.file.PixelSpacing[0] != null
//         //         && app.file.PixelSpacing[1] != null) {
//         //         length = length * app.file.PixelSpacing[0];
//         //     }

//         //     var rulerYUnit = 'mm';
//         //     if (!app.file.PixelSpacing) {
//         //         rulerYUnit = "";
//         //     } else {
//         //         switch (rulerYUnit) {
//         //             case "cm":
//         //                 length = length / 10;
//         //                 break;
//         //             case "mm":
//         //                 break;
//         //             case "inch":
//         //                 length = length / 25.4;
//         //                 break;
//         //             case "um":
//         //                 length = length * 1000;
//         //             default:
//         //                 break;
//         //         }
//         //     }
//         //     text = "Length: " + length.toString() + " " + rulerYUnit;
//         // }
//         // this.textArr = text.split("\n");
//         // this.text_width = this.getTextWidth(this.textArr, app, image);
//         // this.text_height = image.textConfig.getHeight() * this.textArr.length;
//     };

//     isHitText(cpos) {
//         if (cpos.x > this.text_cstart.x && cpos.x < this.text_cstart.x + this.text_width &&
//             cpos.y > this.text_cstart.y && cpos.y < this.text_cstart.y + this.text_height)
//             return true;
//         return false;
//     };

//     drawControlPoints(overlaycanvas) {
//         var len = this.cp.length;
//         for (var i = 0; i < len; i++) {
//             this.drawRectPoint(this.cp[i], overlaycanvas);
//         }
//     };

//     drawRectPoint(p, overlaycanvas) {
//         var canvas = overlaycanvas;
//         var ctx = canvas.getContext("2d");
//         if (!ctx) return;
//         var radius = 3;
//         ctx.save();
//         ctx.translate(0.5, 0.5);
//         ctx.beginPath();
//         ctx.lineWidth = 1;
//         ctx.shadowBlur = 0.5;
//         ctx.shadowOffsetX = 1;
//         ctx.shadowOffsetY = 1;
//         ctx.shadowColor = "black";
//         ctx.strokeStyle = "yellow";
//         ctx.strokeRect(Math.round(p.x) - radius, Math.round(p.y) - radius, 2 * radius, 2 * radius);
//         ctx.restore();
//     };

//     getTextWidth(textArr, overlaycanvas) {
//         var curCanvas = overlaycanvas;
//         var ctx = curCanvas.getContext("2d");
//         ctx.save();
//         ctx.textBaseline = "Top";
//         var max = 0;
//         var len = textArr.length;
//         for (var i = 0; i < len; i++) {
//             var width = ctx.measureText(textArr[i]).width;
//             if (width > max && width <= 150) {
//                 max = width;
//             } else if (width > max && width > 150) {
//                 max = 150;

//             }
//         }
//         ctx.restore();
//         return max;
//     };

//     distance = function (v, w) {
//         let dist = Math.sqrt(v.x - w.x) + Math.sqrt(v.y - w.y);
//         return Math.sqrt(dist);
//     };

//     //移动图元控制点时处理超出cell的情况
//     calculatePointOnBorderForOverlay(mouseStartX, mouseStartY, mouseEndX, mouseEndY, controlPointX, controlPointY, app, boundarieArrays) {
//         var canvas = app.overlaycanvas || app.canvas;
//         var maxWidth = canvas.width - 1;
//         var maxHeight = canvas.height - 1;
//         var minWidth = 0;
//         var minHeight = 0;
//         var minGap = 1;
//         if (boundarieArrays) {
//             if (boundarieArrays[0]) minWidth = boundarieArrays[0];
//             if (boundarieArrays[1]) minHeight = boundarieArrays[1];
//             if (boundarieArrays[2]) maxWidth = boundarieArrays[2];
//             if (boundarieArrays[3]) maxHeight = boundarieArrays[3];
//         }

//         controlPointX += mouseEndX - mouseStartX;
//         controlPointY += mouseEndY - mouseStartY;
//         if (mouseEndX < mouseStartX) { //左移
//             //左移超出左边界
//             if (controlPointX <= minWidth) {
//                 controlPointX = minWidth + minGap;
//             }
//             //超出右边界左移
//             if (mouseEndX > maxWidth) {
//                 controlPointX = maxWidth - minGap;
//             } else {
//                 if (mouseStartX > maxWidth) {
//                     controlPointX = controlPointX + mouseStartX - maxWidth;
//                 }
//             }
//         } else {
//             //超出左边界右移
//             if (mouseEndX < minWidth) {
//                 controlPointX = minWidth + minGap;
//             } else {
//                 if (mouseStartX < minWidth) {
//                     controlPointX = controlPointX + mouseStartX - minWidth;
//                 }
//             }
//             //右移超出右边界
//             if (controlPointX >= maxWidth) {
//                 controlPointX = maxWidth - minGap;
//             }
//         }

//         if (mouseEndY < mouseStartY) {  //上移
//             //上移超出上边界
//             if (controlPointY <= minHeight) {
//                 controlPointY = minHeight + minGap;
//             }
//             //超出下边界上移
//             if (mouseEndY > maxHeight) {
//                 controlPointY = maxHeight - minGap;
//             } else {
//                 if (mouseStartY > maxHeight) {
//                     controlPointY = controlPointY + mouseStartY - maxHeight;
//                 }
//             }
//         } else {
//             //超出上边界下移
//             if (mouseEndY <= minHeight) {
//                 controlPointY = minHeight + minGap;
//             } else {
//                 if (mouseStartY < minHeight) {
//                     controlPointY = controlPointY + mouseStartY - minHeight;
//                 }
//             }
//             //下移超出下边界
//             if (controlPointY > maxHeight) {
//                 controlPointY = maxHeight - minGap;
//             }
//         }
//         var point = [controlPointX, controlPointY];
//         return point;
//     }

// }