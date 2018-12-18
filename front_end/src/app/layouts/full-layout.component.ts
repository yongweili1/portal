import { Component, OnInit, ViewEncapsulation } from '@angular/core';
import { MenuItem } from 'primeng/api';
import { CustomContextMenu } from '../models/contextmenu';

@Component({
    selector: 'mpt-full-layout',
    templateUrl: './full-layout.component.html',
    styleUrls: ['./layout.component.less'],
    encapsulation: ViewEncapsulation.None
})
export class FullLayoutComponent implements OnInit {

    private items: MenuItem[];
    public disabled = false;
    public status: { isopen: boolean } = { isopen: false };
    cmModel: CustomContextMenu;

    public toggled(open: boolean): void { }

    public toggleDropdown($event: MouseEvent): void {
        $event.preventDefault();
        $event.stopPropagation();
        this.status.isopen = !this.status.isopen;
    }

    ngOnInit() {
        this.cmModel = new CustomContextMenu();
        this.items = this.cmModel.getMenItems();
    }

}

