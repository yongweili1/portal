import { LazyExcuteHandler } from "../../contouring/lazy_excute_handler";

export class BaseAction {
    lazyExcuteHandler: LazyExcuteHandler;
    tag: string;
    
    constructor () {
        this.lazyExcuteHandler = new LazyExcuteHandler();
    }

    handleMouseDown(evt) {}

    handleMouseMove(evt) {}

    handleMouseUp(evt) {}
}