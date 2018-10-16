import { Directive, ElementRef, Input, HostListener, OnInit, Output, EventEmitter } from '@angular/core';
import { ConMessageService } from '../service/conMessage.service';
import { SharpFactory } from '../tools/sharpfactory'
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
    isPaint: boolean = false;
    isFirstClick: boolean = true;
    isMousedown: boolean = true;
    myContext: CanvasRenderingContext2D;
    myStage: any;
    backStage: any;
    labelTxt: string;
    sharp: any;
    curTarget: any;
    @Input() backCanvas;

    constructor(private el: ElementRef, private contouringService: ConMessageService) { }

    ngOnInit() {
        this.myContext = this.el.nativeElement.getContext("2d");
        this.myStage = new createjs.Stage(this.el.nativeElement);
        
        createjs.Touch.enable(this.myStage);
        this.myStage.enableMouseOver(50);
        this.myStage.mouseMoveOutside = true;
        this.myStage.autoClear = false;

        this.myContext.strokeStyle = this.contourColor;
        this.myContext.lineWidth = this.contourLineWidth;

        this.contouringService.curAction$.subscribe(curAction => {
            this.curAction = curAction
        })

    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        this.isMousedown = true;
        this.startX = event.offsetX;
        this.startY = event.offsetY;
        this.startPoint = new Point(event.offsetX, event.offsetY);
        this.sharp = SharpFactory.getInstance().sharp(this.curAction, this.myStage);
        if (typeof(this.sharp) != 'undefined') {
            this.sharp.setStartPoint(this.startPoint);
        }
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.isMousedown && typeof(this.sharp) != 'undefined') {
            this.isPaint = true;
            this.curX = event.offsetX;
            this.curY = event.offsetY;
            this.sharp.setEndPoint(new Point(this.curX, this.curY));
            this.sharp.update();
        }
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        if (this.isPaint) {
            this.sharp.update();
        }
        this.isMousedown = false;
        this.isPaint = false;
    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        this.onMouseUp(event);
    }

    @HostListener('dblclick', ['$event']) onDbClick(event: MouseEvent) {
        this.isMousedown = false;
        this.curAction = "";
        this.contouringService.SetCurAction("quitDrawPri");
    }
}