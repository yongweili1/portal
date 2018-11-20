import { Pan } from "./pan";
import { Zoom } from "./zoom";
import { Rotate } from "./rotate";
import { WWWL } from "./wwwl";
import { Locate } from './locate';

declare var actions: any;

export class Action {
    private _action: any;
    private _tag: any;

    constructor(tag) {
        this._tag = tag;
    }

    set(action) {
        if (action == actions.pan) {
            this._action = new Pan(this._tag);
        } else if (action == actions.zoom) {
            this._action = new Zoom(this._tag);
        } else if (action == actions.rotate) {
            this._action = new Rotate(this._tag);
        } else if (action == actions.window) {
            this._action = new WWWL(this._tag);
        } else if (action == actions.locate) {
            this._action = new Locate(this._tag);
        } else { // default: pan
            this._action = new Pan(this._tag);
        }
    }

    mouseDown(evt) {
        if (this._action === undefined) {
            return;
        }
        this._action.handleMouseDown(evt);
    }

    mouseMove(evt) {
        if (this._action === undefined) {
            return;
        }
        this._action.handleMouseMove(evt);
    }

    mouseUp(evt) {
        if (this._action === undefined) {
            return;
        }
        this._action.handleMouseUp(evt);
    }
}
