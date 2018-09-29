import { Directive, HostListener } from '@angular/core';

@Directive({
  selector: '.sidebar-toggler',
})
export class SidebarToggleDirective {
  constructor() {
    // TODO
  }

  @HostListener('click', ['$event'])
  private toggleOpen($event: any) {
    $event.preventDefault();
    document.querySelector('.layout-menu-static').classList.toggle('layout-menu-static-inactive');
  }
}
