import { Routes } from '@angular/router';
import { ForgotPasswordComponent } from './base/forgot-password/forgot-password.component';
import { LoginComponent } from './base/login/login.component';
import { NoContentComponent } from './base/no-content/no-content.component';
import { RenewPasswordComponent } from './base/renew-password/renew-password.component';
import { ContouringComponent } from './components/contouring/contouring.component';
import { HomeComponent } from './components/home/home.component';
import { PublicPageGuard } from './core/public-page.guard';
import { FullLayoutComponent } from './layouts/full-layout.component';
import { SimpleLayoutComponent } from './layouts/simple-layout.component';


/* tslint:disable:max-line-length */
export const ROUTES: Routes = [
    {
        path: '',
        component: FullLayoutComponent,
        children: [
            {
                path: '',
                redirectTo: 'home',
                pathMatch: 'full',
            },
            {
                path: 'home',
                component: HomeComponent,
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
                        component: ContouringComponent,
                    },
                    {
                        path: 'upload',
                        loadChildren: './components/upload/upload.module#UploadModule'
                    },
                    {
                        path: 'pacs',
                        loadChildren: './components/pacs/pacs.module#PacsModule',
                    },
                    {
                        path: 'patient-template',
                        loadChildren: './components/patient-template/patient-template.module#PaListModule',
                    },
                    {
                        path: 'training-status',
                        loadChildren: './components/training-status/training-status.module#TrainingStatusModule',
                    },
                    {
                        path: 'setting',
                        loadChildren: './components/setting/setting.module#SettingModule',
                    }
                ]
            },
        ]
    },

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
/* tslint:enable:max-line-length */
