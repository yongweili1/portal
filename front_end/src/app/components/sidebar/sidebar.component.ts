import {Component, OnInit} from '@angular/core';
import {Router} from '@angular/router';
import {AuthService} from '../../core/auth.service';

@Component({
  selector: 'mpt-sidebar',
  templateUrl: 'sidebar.component.html',
  styleUrls: ['./sidebar.component.less']
})
export class SideBarComponent implements OnInit {

  private loggedIn: boolean;
  private isMenuHidden = true;

  public menuState = {};

  public menuData = [
   {
      link: '/base/patient-template',
      icon: 'fa fa-fw fa-lg fa-list-ul',
      title: 'united-imaging.Package.title',
      resName: 'base.patient-template'
    },
    {
      name: 'system-management',
      icon: 'fa fa-fw fa-lg fa-cog',
      title: 'united-imaging.system-management.title',
      items: [  {
        link: '/base/contouring',
        title: 'base.users-mgnt.title',
        resName: 'base.users.query'
      }
      ]
    },
    // {
    //   name: 'Setting',
    //   icon: 'fa fa-fw fa-lg fa-tasks',
    //   link: '/base/setting',
    //   title: 'Setting',
    // }
    {
      name: 'Setting',
      icon: 'fa fa-fw fa-lg fa-tasks',
      title: 'Setting',
      items: [  {
        link: '/base/setting/UserSetting',
        title: 'UserSetting',
        resName: 'base.users.query'
      },
      {
        link: '/base/setting/SysSetting',
        title: 'SysSetting',
        resName: 'base.users.query'
      }
      ]
    }
  ];

  public constructor(
    private router: Router,
    private authService: AuthService) {
  }

  public ngOnInit(): void {
    this.loggedIn = this.authService.loggedIn();

    this.authService.events.subscribe(() => {
      this.loggedIn = this.authService.loggedIn();
    });

  }

  public onToggleMenu(e: Event, menuId) {
    this.menuState[menuId] = true;
    for (const _menuId in this.menuState) {
      if (_menuId !== menuId) {
        this.menuState[_menuId] = false;
      }
    }

    document.querySelector('.layout-menu-static').classList.remove('layout-menu-static-inactive');

    e.stopPropagation();
  }

}
