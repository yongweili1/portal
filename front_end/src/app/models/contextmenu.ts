import { MenuItem } from 'primeng/api';

export class CustomContextMenu {
    private menu: MenuItem = {
        label: 'delete',
        routerLink: '',
    };
    private menus: MenuItem[] = [this.menu];
    private items: MenuItem[] = [
        // {
        //   label: 'File',
        //   items: [{
        //     label: 'New',
        //     icon: 'pi pi-fw pi-plus',
        //     items: [
        //       { label: 'Project' },
        //       { label: 'Other' },
        //     ]
        //   },
        //   { label: 'Open' },
        //   { label: 'Quit' }
        //   ]
        // },
        // {
        //   label: 'Edit',
        //   icon: 'pi pi-fw pi-pencil',
        //   items: [
        //     { label: 'Delete', icon: 'pi pi-fw pi-trash' },
        //     { label: 'Refresh', icon: 'pi pi-fw pi-refresh' }
        //   ]
        // }
    ];

    public setMenuItems(_menus: MenuItem[]): MenuItem[] {
        this.menus = _menus;
        return this.menus;
    }

    public clearMenuItems() {
        this.menus = [];
    }

    public addMenuItem(_menu: MenuItem): MenuItem[] {
        return this.menus.concat(_menu);
    }

    public getMenItems(): MenuItem[] {
        return this.menus;
    }
}
