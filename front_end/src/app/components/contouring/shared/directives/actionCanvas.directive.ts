import { Directive, ElementRef, HostListener, Input, OnInit, SimpleChanges } from '@angular/core';
import { KeyValuePair } from '../../../../shared/common/keyvaluepair';
import { Action } from '../actions/action';
import { ExcuteHelper } from "../../../contouring/shared/tools/excute_helper";

declare var actions: any;

@Directive({
    selector: '[action-canvas]'
})
export class ActionCanvasDirective implements OnInit {
    excuteHelper: ExcuteHelper;
    action: Action;
    isMouseDown: boolean;

    @Input() tag;
    @Input() actionInfo: KeyValuePair;

    constructor(private el: ElementRef) { }

    ngOnInit() {
        console.log('[action-canvas]ngOnInit');
        this.excuteHelper = new ExcuteHelper();
        this.actionInfo = new KeyValuePair(actions.locate);
        this.action = new Action(this.tag);
    }

    ngOnChanges(changes: SimpleChanges) {
        if (this.actionInfo === undefined) {
            console.log('ActionInfo is wrong.')
            return;
        }
        console.log('[action-canvas]Current action is ' + this.actionInfo.key());
        if (this.action !== undefined) {
            this.action.set(this.actionInfo.key());
        }
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
        if (!this.excuteHelper.canExcuteByCount()) {
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
