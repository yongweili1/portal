import { Directive, ElementRef, HostListener, Input, OnInit } from '@angular/core';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
import { EventAggregator } from '../../../../shared/common/event_aggregator';
import { Action } from '../actions/action';
import { LazyExcuteHandler } from "../../../contouring/lazy_excute_handler";

declare var actions: any;

@Directive({
    selector: '[action-canvas]'
})
export class ActionCanvasDirective implements OnInit {
    lazyExcuteHandler: LazyExcuteHandler;
    actionInfo: KeyValuePair;
    action: Action;
    isMouseDown: boolean;

    @Input() name;

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[action-canvas]ngOnInit');
        this.lazyExcuteHandler = new LazyExcuteHandler();
        this.actionInfo = new KeyValuePair(actions.locate);
        this.action = new Action(this.name);

        EventAggregator.Instance().actionInfo.subscribe(actionInfo => {
            if (actionInfo == null) {
                console.log('ActionInfo is wrong.')
                return;
            }
            console.log('[action-canvas]Current action is ' + actionInfo.key());
            this.actionInfo = actionInfo;

            this.action.set(this.actionInfo.key());
        });
    }

    @HostListener('mousedown', ['$event']) onMouseDown(event: MouseEvent) {
        console.log('[action-canvas]mousedown');
        this.isMouseDown = true;
        this.action.mouseDown(event);
    }

    @HostListener('mousemove', ['$event']) onMouseMove(event: MouseEvent) {
        console.log('[action-canvas]mousemove');
        if (!this.isMouseDown) {
            return;
        }
        if (!this.lazyExcuteHandler.canExcuteByCount()) {
            this.action.mouseDown(event);
            return;
        }
        this.action.mouseMove(event);
    }

    @HostListener('mouseup', ['$event']) onMouseUp(event: MouseEvent) {
        console.log('[action-canvas]mouseup');
        this.isMouseDown = false;
        this.action.mouseUp(event);
    }

    @HostListener('mouseleave', ['$event']) onMouseLeave(event: MouseEvent) {
        console.log('[action-canvas]mouseleave');
    }

    @HostListener('dblclick', ['$event']) onDbClick(event: MouseEvent) {
        console.log('[action-canvas]dblclick');
    }

}