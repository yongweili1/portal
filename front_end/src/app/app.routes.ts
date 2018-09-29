import { Routes } from '@angular/router';

import { PublicPageGuard } from './core/public-page.guard';

import { LoginComponent } from './base/login/login.component';
import { ForgotPasswordComponent } from './base/forgot-password/forgot-password.component';

import { NoContentComponent } from './base/no-content/no-content.component';

import { FullLayoutComponent } from './layouts/full-layout.component';
import { SimpleLayoutComponent } from './layouts/simple-layout.component';
import { RenewPasswordComponent } from './base/renew-password/renew-password.component';
import {ContouringComponent } from './components/tps/contouring/contouring.component'

/* tslint:disable:max-line-length */
export const ROUTES: Routes = [
  {
    path: '',
    component: FullLayoutComponent,
    children: [
      {
        path: '',
        loadChildren: './components/home/home.module#HomeModule',
      },
      {
        path: 'home',
        loadChildren: './components/home/home.module#HomeModule',
      },
      {
        path: 'change-password',
        loadChildren: './base/change-password/change-password.module#ChangePasswordModule',
      },
      {
        path: 'base',
        children: [
          {
            path: 'contouring',
            // loadChildren: './components/tps/contouring/contouring.module#ContouringModule',
            // data: {
            //   res: ' '
            // },
            component: ContouringComponent,
          },
          {
            path: 'patient-template',
            loadChildren: './components/patient-template/patient-template.module#PaListModule',
          },
          {
            path: 'setting',
            loadChildren: './components/setting/setting.module#SettingModule',
          }
        ]
      },
    ]
  },
  // {
  //   path: '',
  //   redirectTo: 'index',
  //   pathMatch: 'full',
  // },
  {
    path: '',
    component: SimpleLayoutComponent,
    data: {
      title: 'Pages'
    },
    children: [
      {
        path: 'login',
        component: LoginComponent,
        canActivate: [PublicPageGuard]
      },
      {
        path: 'forgot-password',
        component: ForgotPasswordComponent,
        canActivate: [PublicPageGuard]
      },
      {
        path: 'renew-password',
        component: RenewPasswordComponent,
        canActivate: [PublicPageGuard]
      },
    ]
  },
  {
    path: '**',
    component: NoContentComponent
  }
];

// export const ROUTES: Routes = [
//   {
//     path: '',
//     redirectTo: 'index',
//     pathMatch: 'full',
//   },
//   {
//     path: 'index',
//     component: FullLayoutComponent,
//     children: [
//       {
//         path: '',
//         loadChildren: './components/home/home.module#HomeModule',
//       },
//       {
//         path: 'home',
//         loadChildren: './components/home/home.module#HomeModule',
//       }
//     ]
//   },
//   {
//     path: '**',
//     component: NoContentComponent
//   }
// ];
/* tslint:enable:max-line-length */
