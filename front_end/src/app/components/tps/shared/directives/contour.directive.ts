import { Directive, ElementRef, Input, HostListener, OnInit ,Output, EventEmitter } from '@angular/core';
import { ConMessageService } from '../service/conMessage.service';

@Directive({
    selector: '[myContour]'
})

export class ContourDirective implements OnInit {

    curAction: string;
    // curAction: string = "combo";
    canvasLeft: number;
    canvasTop: number;
    clickXs: Array<number> = Array<number>();
    clickYs: Array<number> = Array<number>();
    startX: number;
    startY: number;
    curX: number;
    curY: number;
    radius: number;
    contourColor: string = "#00ffff";
    contourLineWidth = 2;
    isPaint: boolean = false;
    isFirstClick: boolean = true;
    isMousedown: boolean = true;
    myContext: CanvasRenderingContext2D;
    backContext: CanvasRenderingContext2D;
    labelTxt: string;
    @Input() backCanvas;
    @Output() quitDraw: EventEmitter<any> = new EventEmitter<any>();

    constructor(private el: ElementRef, private contouringService: ConMessageService) { }

    ngOnInit() {

        this.myContext = this.el.nativeElement.getContext("2d");
        this.backContext = this.backCanvas.getContext("2d");

        this.backContext.strokeStyle = this.contourColor;
        this.backContext.lineWidth = this.contourLineWidth;

        this.myContext.strokeStyle = this.contourColor;
        this.myContext.lineWidth = this.contourLineWidth;
        

        this.contouringService.curAction$.subscribe(
            curAction => this.curAction = curAction
        )
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        // if (this.isFirstClick) {
        //     this.startX = event.offsetX;
        //     this.startY = event.offsetY;
            this.isMousedown = true;
            this.startX = event.offsetX;
            this.startY = event.offsetY;
        // }

        if (this.curAction == "rectangle") {

        }

        if (this.curAction == "circle") {

        }
        if (this.curAction == "measure") {

        }

        if (this.curAction == "combo") {
            if (this.isFirstClick) {
                this.el.nativeElement.style.cursor = "url('../../../../assets/images/pen.cur'),auto";
                this.backContext.beginPath();
                this.backContext.moveTo(this.startX, this.startY)
            }
            else {
                this.backContext.lineTo(event.offsetX, event.offsetY);
                this.backContext.stroke();
            }

        }

    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        if (this.isMousedown) {
            this.isPaint = true;
            this.curX = event.offsetX;
            this.curY = event.offsetY;
            if (this.curAction == "rectangle") {
                this.myContext.clearRect(0, 0, this.backContext.canvas.width, this.backContext.canvas.height);
                this.myContext.beginPath();
                this.myContext.strokeRect(this.startX, this.startY, this.curX - this.startX, this.curY - this.startY);
                this.myContext.closePath();
            }

            if (this.curAction == "combo") {

                this.backContext.lineTo(this.curX, this.curY)
                this.backContext.stroke();

            }
            if (this.curAction == "measure") {
                this.myContext.clearRect(0, 0, this.backContext.canvas.width, this.backContext.canvas.height);
                this.myContext.beginPath();
                this.myContext.moveTo(this.startX,this.startY);
                this.myContext.lineTo(this.curX, this.curY);
                this.myContext.stroke();
                this.myContext.closePath();
                
            }

            if (this.curAction == "circle") {
                this.myContext.beginPath();
                this.radius = Math.sqrt((this.curX - this.startX) * (this.curX - this.startX) + (this.curY - this.startY) * (this.curY - this.startY));
                this.myContext.arc(this.startX, this.startY, this.radius, 0, 2 * Math.PI);
                this.myContext.stroke();
                this.myContext.closePath();

            }
        }
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        if (this.isPaint){
            this.myContext.clearRect(0, 0, this.backContext.canvas.width, this.backContext.canvas.height);
            if (this.curAction == "rectangle") {
                this.backContext.strokeRect(this.startX, this.startY, this.curX - this.startX, this.curY - this.startY);  
            }
            if (this.curAction == "measure") {
                this.backContext.moveTo(this.startX,this.startY);
                this.backContext.lineTo(this.curX, this.curY);
                this.backContext.stroke();
            }
    
            if (this.curAction == "circle") {
                this.backContext.beginPath();
                this.backContext.arc(this.startX, this.startY, this.radius, 0, 2 * Math.PI);
                this.backContext.stroke();
                this.backContext.closePath();
    
            }
        }
        this.isMousedown = false;
        this.isPaint = false;

    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        this.onMouseUp(event);
    }

    @HostListener('dblclick', ['$event']) onDbClick(event: MouseEvent) {
        this.backContext.closePath();
        this.backContext.stroke();
        this.isMousedown = false;
        this.curAction = "";
        this.contouringService.SetCurAction("quitDrawPri");
    }
}