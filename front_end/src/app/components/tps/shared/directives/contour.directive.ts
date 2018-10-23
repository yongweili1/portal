import { Directive, ElementRef, Input, HostListener, OnInit, Output, EventEmitter } from '@angular/core';
import { ConMessageService } from '../service/conMessage.service';
import { CircleFactory } from '../tools/factory/circle-factory'
import { LineFactory } from '../tools/factory/line-factory'
import { RectangleFactory } from '../tools/factory/rectangle-factory'
import { FreepenFactory } from '../tools/factory/freepen-factory'
import { CrosslineFactory } from '../tools/factory/crossline-factory'
import { Point } from '../tools/point'

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
    @Input() viewName;

    constructor(private el: ElementRef, private contouringService: ConMessageService) { }

    ngOnInit() {
        this.myContext = this.el.nativeElement.getContext("2d");
        this.myStage = new createjs.Stage(this.el.nativeElement);
        
        createjs.Touch.enable(this.myStage);
        this.myStage.enableMouseOver();
        this.myStage.mouseMoveOutside = true;
        this.myStage.autoClear = false;

        this.myContext.strokeStyle = this.contourColor;
        this.myContext.lineWidth = this.contourLineWidth;
        

        let crossline = CrosslineFactory.getInstance().createSharpContainer(this.myStage);
        crossline.setHorizontal()
        crossline.setVertical()
        crossline.update()

        this.contouringService.curAction$.subscribe(curAction => {
            console.log('curAction')
            this.curAction = curAction
        })
        let that = this; 
        this.contouringService.crossPoint$.subscribe(crossPoint => {
            if (crossPoint['view'] == this.viewName) {
                let p = crossPoint['point']
                let cross;
                for (let index = 0; index < this.myStage.children.length; index++) {
                    const shape = this.myStage.children[index];
                    if (shape.type == 'crossline') {
                        cross = shape;
                        break;
                    }
                }
                cross.setCenter(p)
                cross.setHorizontal()
                cross.setVertical()
                cross.update()
                console.log('update', this.viewName)
            }
        })
    }
    
    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        console.log('mousedown')
        this.sharp = this.getShapeContainerInstance(this.curAction, this.myStage);
        if (this.sharp != null)
            this.sharp.handleMouseDown(event)
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.sharp != null)
            this.sharp.handleMouseMove(event)
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
        }
    }
}