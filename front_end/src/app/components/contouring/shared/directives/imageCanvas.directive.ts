import { Directive, ElementRef, Input, OnInit } from '@angular/core';

@Directive({
    selector: '[image-canvas]'
})
export class ImageCanvasDirective implements OnInit {
    @Input() name;
    @Input() data;
    canvas: any;
    context: CanvasRenderingContext2D;

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[image-canvas]ngOnInit');
        this.canvas = this.el.nativeElement;
        this.context = this.el.nativeElement.getContext("2d");
    }

    ngOnChanges() {
        if (this.data === undefined) {
            return;
        }
        this.update();
    }

    private update() {
        const img = new Image();
        const base64Header = 'data:image/jpeg;base64,';
        const imgData1 = base64Header + this.data;
        img.src = imgData1;
        const that = this;
        img.onload = function () {
            that.context.clearRect(0, 0, that.canvas.width, that.canvas.height);
            that.context.drawImage(img, 0, 0, that.canvas.width, that.canvas.height);
        };
    }
}
