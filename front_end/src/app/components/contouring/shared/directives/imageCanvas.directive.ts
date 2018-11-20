import { Directive, ElementRef, Input, OnInit, SimpleChanges } from '@angular/core';

@Directive({
    selector: '[image-canvas]'
})
export class ImageCanvasDirective implements OnInit {
    @Input() tag;
    @Input() base64;
    canvas: any;
    context: CanvasRenderingContext2D;

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[image-canvas]ngOnInit');
        this.canvas = this.el.nativeElement;
        this.context = this.el.nativeElement.getContext("2d");
    }

    ngOnChanges(changes: SimpleChanges) {
        if (this.base64 === undefined) {
            return;
        }
        this.update();
    }

    private update() {
        const img = new Image();
        const base64Header = 'data:image/jpeg;base64,';
        const imgData = base64Header + this.base64;
        img.src = imgData;
        const that = this;
        img.onload = function () {
            that.context.clearRect(0, 0, that.canvas.width, that.canvas.height);
            that.context.drawImage(img, 0, 0, that.canvas.width, that.canvas.height);
        };
    }
}