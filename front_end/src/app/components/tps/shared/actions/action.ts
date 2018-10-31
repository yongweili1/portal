import { Pan } from "./pan";
import { Zoom } from "./zoom";
import { Rotate } from "./rotate";
import { WWWL } from "./wwwl";
import { Locate } from './locate';

declare var actions: any;

export class Action {
    private static _instance: Action;
    static Handler() {
        if (Action._instance == null)
            Action._instance = new Action()
        return this._instance;
    }

    private _action: any;

    private constructor() {}

    setAction(action) {
        if (action == actions.pan) {
            this._action = new Pan()
        } else if (action == actions.zoom) {
            this._action = new Zoom()
        } else if (action == actions.rotate) {
            this._action = new Rotate()
        } else if (action == actions.wwwl) {
            this._action = new WWWL()
        } else if (action == actions.locate) {
            this._action = new Locate()
        } else {

        }
    }

    mouseDown(evt) {
        this._action.handleMouseDown(evt);
    }

    mouseMove(evt) {
        this._action.handleMouseMove(evt);
    }

    mouseUp(evt) {
        this._action.handleMouseUp(evt);
    }
}
