import { BaseContainer } from "./base_container";
import { Fader } from "../overlay/fader";
import { Point } from "../tools/point";
import { EventAggregator } from '../../../../shared/common/event_aggregator';

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

	handleMouseDown(evt) {
		super.handleMouseDown(evt)
		this.isMousedown = true;
		this.currentMouseButton = evt.button;

		this.prePos = new Point(evt.offsetX, evt.offsetY)
		this.fader.center = this.prePos;
		this.update();
    }
    handleMouseMove(e) {
		this.fader.center = new Point(e.offsetX, e.offsetY);
		this.fader.update(this.fader.center)

        if (this.isMousedown) {
			let curPos = new Point(e.offsetX, e.offsetY)
			if (this.currentMouseButton == 1) {  // middle button
				if (curPos.y - this.prePos.y < 0) {
					if (this.fader.radius < 80)
						this.fader.radius += 1;
				} else {
					if (this.fader.radius > 5)
						this.fader.radius -= 1;
				}
			} else if (this.currentMouseButton == 2) {  // right button

			} else {  // left button
				// check collision
				// clipper
				console.log('clipper', this.parent.name)
				EventAggregator.Instance().clipInfo.publish([this.parent.name, this.fader.cps]);
			}
			this.prePos = curPos;

            this.update();
        }
    }
    handleMouseUp(e) {
		this.isMousedown = false;
		this.currentMouseButton = -1;
        this.isPaint = false;
    }

    handlePressMove(e) {
        this.fader.center = new Point(e.offsetX, e.offsetY);
        this.update();
	}

	handleMouseLeave(e) {
	}
}
