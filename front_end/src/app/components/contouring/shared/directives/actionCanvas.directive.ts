import { Directive, ElementRef, HostListener, Input, OnInit } from '@angular/core';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
import { ConMessageService } from '../service/conMessage.service';
import { EventAggregator } from '../../../../shared/common/event_aggregator';

declare var createjs: any;
declare var actions: any;
declare var shapes: any;

@Directive({
    selector: '[action-canvas]'
})
export class ActionCanvasDirective implements OnInit {
    actionInfo: KeyValuePair;
    myContext: CanvasRenderingContext2D;

    @Input() name;

    constructor(private el: ElementRef, private contouringService: ConMessageService) { }

    ngOnInit() {
        this.myContext = this.el.nativeElement.getContext("2d");
        this.actionInfo = new KeyValuePair(actions.locate);

        EventAggregator.Instance().actionInfo.subscribe(actionInfo => {
            if (actionInfo == null) {
                console.log('ActionInfo is wrong.')
                return;
            }
            console.log('Current action is ' + actionInfo.key());
            this.actionInfo = actionInfo;
        });
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {

    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {

    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {

    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
    }

    @HostListener('dblclick', ['$event']) onDbClick(event: MouseEvent) { }

}
