import { Directive, ElementRef, Input, HostListener, OnInit, Output, EventEmitter } from '@angular/core';
import { ConMessageService } from '../service/conMessage.service';
import { CircleFactory } from '../tools/factory/circle-factory'
import { LineFactory } from '../tools/factory/line-factory'
import { RectangleFactory } from '../tools/factory/rectangle-factory'
import { FreepenFactory } from '../tools/factory/freepen-factory'
import { Point } from '../tools/point'
import { FreepenContainer } from '../container/freepen_container';

declare var createjs: any;

@Directive({
    selector: '[myContour]'
})
export class ContourDirective implements OnInit {
    curAction: string;
    canvasLeft: number;
    canvasTop: number;
    clickXs: Array<number> = Array<number>();
    clickYs: Array<number> = Array<number>();
    startX: number;
    startY: number;
    startPoint: Point;
    curX: number;
    curY: number;
    radius: number;
    contourColor: string = "#00ffff";
    contourLineWidth = 2;
    isFirstClick: boolean = true;
    myContext: CanvasRenderingContext2D;
    myStage: any;
    backStage: any;
    labelTxt: string;
    sharp: any;
    curTarget: any;
    @Input() backCanvas;
    @Input() name;

    constructor(private el: ElementRef, private contouringService: ConMessageService) { }

    ngOnInit() {
        this.myContext = this.el.nativeElement.getContext("2d");
        this.myStage = new createjs.Stage(this.el.nativeElement);
        this.curAction = 'croselect'
        
        createjs.Touch.enable(this.myStage);
        this.myStage.enableMouseOver();
        this.myStage.mouseMoveOutside = true;
        this.myStage.autoClear = false;

        this.myContext.strokeStyle = this.contourColor;
        this.myContext.lineWidth = this.contourLineWidth;
        
        this.contouringService.curAction$.subscribe(curAction => {
            if (curAction == 'clearAllShape') {
                this.myStage.removeAllChildren()
            } else {
                this.curAction = curAction
            }
        })

        this.contouringService.graphics$.subscribe(data => {
            if (this.name != data[0]) return;
            this.myStage.removeAllChildren()
            this.myStage.clear()
            let graphics = data[1]
            let contours = []
            if (graphics == null || graphics.length == 0) return;
            graphics.forEach(graphic => {
                if (graphic == null || graphic.length == 0) return;
                graphic.forEach(shape => {
                    let contour = []
                    shape.forEach(cp => {
                        contour.push(new Point(cp[0][0], cp[0][1]))
                    });
                    contours.push(contour)
                });
            });
            // draw graphics
            contours.forEach(cps => {
                cps.push(cps[0].copy())
                let freepen = new FreepenContainer(this.myStage)
                freepen.cps = cps
                freepen.update()
            });
        })
    }
    
    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        let info = 'MouseDown info: '
        info += 'xy: (' + event.x + ', ' + event.y + ')'
        info += ' client: (' + event.clientX + ', ' + event.clientY + ')'
        info += ' offset: (' + event.offsetX + ', ' + event.offsetY + ')'
        info += ' screen: (' + event.screenX + ', ' + event.screenY + ')'
        info += ' layer: (' + event.layerX + ', ' + event.layerY + ')'
        info += ' movement: (' + event.movementX + ', ' + event.movementY + ')'
        document.getElementById('clickInfo').innerText = info
        this.myStage.children.forEach(shape => {
            if (shape.type == 'freepen') {
                shape.editable = this.curAction == 'freepen2' ? true : false;
            }
        });
        this.sharp = this.getShapeContainerInstance(this.curAction, this.myStage);
        if (this.sharp != null)
            this.sharp.handleMouseDown(event)
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        let info = 'MouseMove info: '
        info += 'xy: (' + event.x + ', ' + event.y + ')'
        info += ' client: (' + event.clientX + ', ' + event.clientY + ')'
        info += ' offset: (' + event.offsetX + ', ' + event.offsetY + ')'
        info += ' screen: (' + event.screenX + ', ' + event.screenY + ')'
        info += ' layer: (' + event.layerX + ', ' + event.layerY + ')'
        info += ' movement: (' + event.movementX + ', ' + event.movementY + ')'
        document.getElementById('moveInfo').innerText = info
        if (this.sharp != null){
            this.sharp.handleMouseMove(event)
        }
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        if (this.sharp != null)
            this.sharp.handleMouseUp(event)   
    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        this.onMouseUp(event);
    }

    @HostListener('dblclick', ['$event']) onDbClick(event: MouseEvent) {
        this.curAction = "";
        this.contouringService.SetCurAction("quitDrawPri");
    }
    
    getShapeContainerInstance(curAction:any, stage:any){
        switch(curAction){
            case 'measure':
                return LineFactory.getInstance().createSharpContainer(stage);
            case 'rectangle':
                return RectangleFactory.getInstance().createSharpContainer(stage);
            case 'circle':
                return CircleFactory.getInstance().createSharpContainer(stage);
            case 'freepen':
                return FreepenFactory.getInstance().createSharpContainer(stage);
            default:
                return
        }
    }
}