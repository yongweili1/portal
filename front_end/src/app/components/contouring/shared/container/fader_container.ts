import { BaseContainer } from "./base_container";
import { Fader } from "../overlay/fader";
import { Point } from "../tools/point";

export class FaderContainer extends BaseContainer {
	fader: Fader;
	prePos: Point;

	constructor(stage) {
		super(stage, 'nudge');
		this.fader = new Fader(stage);
		this.addChild(this.fader)
	}

	update() {
		this.fader.update(this.fader.center);
	}

	getCenter() {
		return this.fader.center;
	}

	getCps() {
		return this.fader.cps;
	}

	handleMouseDown(evt) {
		super.handleMouseDown(evt)

		this.prePos = new Point(evt.offsetX, evt.offsetY)
		this.fader.setCenter(this.prePos);
		this.update();
    }

    handleMouseMove(e) {
		this.fader.setCenter(new Point(e.offsetX, e.offsetY));
        this.update();

        if (this.isMousedown) {
			let curPos = new Point(e.offsetX, e.offsetY)
			if (this.currentPressDownBtn == 1) {  // middle button
				if (curPos.y - this.prePos.y < 0) {
					if (this.fader.radius < 80)
						this.fader.radius += 1;
				} else {
					if (this.fader.radius > 5)
						this.fader.radius -= 1;
				}
			}
			this.prePos = curPos;
        }
	}

    handleMouseUp(e) {
		super.handleMouseUp(e);
        this.isPaint = false;
    }

    handlePressMove(e) {
        this.fader.setCenter(new Point(e.offsetX, e.offsetY));
        this.update();
	}

	handleMouseLeave(e) {
	}
}
