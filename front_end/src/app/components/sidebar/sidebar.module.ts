import { NgModule } from '@angular/core';
import { RouterModule } from '@angular/router';
import { SharedModule } from '../../shared/shared.module';
import { SideBarComponent } from './sidebar.component';
import { NAV_DROPDOWN_DIRECTIVES } from '../../shared/directives/nav-dropdown.directive';

@NgModule({
  imports: [
    SharedModule,
    RouterModule
  ],
  declarations: [
    SideBarComponent,
    NAV_DROPDOWN_DIRECTIVES
  ],
  exports: [
    SideBarComponent
  ]
})
export class SideBarModule {
}
