export enum ActionTypeEnum {
    unknown = 0,
    pan,
    zoom,
    rotate,
    locate,
    window,
    nudge,
    select,
    shape
}

export enum ShapeTypeEnum {
    unknown = 0,
    controlpoint,
    text,
    line,
    circle,
    rectangle,
    freepen,
    freepen2,
    fader,
    border,
    ellipse
}

export enum CanvasTypeEnum {
    unknown = 0,
    image,
    overlay,
    action,
    cross
}
