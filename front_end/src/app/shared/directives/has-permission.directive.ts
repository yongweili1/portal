import {
    Directive,
    OnInit, OnDestroy,
    ElementRef, Input, Renderer
} from '@angular/core';
import { AuthService } from '../../core/auth.service';

@Directive({
    selector: '[has-permission]'
})
export class HasPermissionDirective implements OnInit, OnDestroy {
    @Input('role') private role: string;
    @Input('resource') private resource: string;

    constructor(
        private el: ElementRef,
        private renderer: Renderer,
        private authService: AuthService) {
    }

    public ngOnInit() {
        this.checkPermission();
    }

    public ngOnDestroy(): void {
        // TODO
    }

    private checkPermission(): void {
        let onChecked = (res: boolean) => {
            this.renderer.setElementStyle(this.el.nativeElement, 'display', res ? '' : 'none');
        };
        // this.authService.hasPermission(this.role, this.resource).subscribe(onChecked);
    }

}