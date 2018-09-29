import { Component, OnInit, HostListener } from '@angular/core';
import { Router } from '@angular/router';
import { MenuItem } from 'primeng/primeng';
import { TranslateService } from '@ngx-translate/core';
import { AppConfigService } from '../../app.config';
import { AuthService } from '../../core/auth.service';
import { User, UserService } from '../../core/user.service';

@Component({
    selector: 'mpt-header',
    templateUrl: 'header.component.html',
    styleUrls: ['./header.component.less'],
})

export class HeaderComponent implements OnInit {

    public settings: MenuItem[];

    public loggedIn: boolean;
    public isMenuHidden = true;
    private user: User;

    public langs: MenuItem[];

    private langLabels: {};

    public currentLang: any;

    public themes: MenuItem[];

    private themeLabels: {};

    public currentTheme: any;

    constructor(
        private router: Router,
        private authService: AuthService,
        private userService: UserService,
        private appConfig: AppConfigService,
        private translate: TranslateService) {
    }

    public ngOnInit(): void {

        this.loggedIn = this.authService.loggedIn();
        if (this.loggedIn) {
            this.userService.me().subscribe((user) => this.user = user);
        }
        this.authService.events.subscribe(() => {
            this.loggedIn = this.authService.loggedIn();
        });

        this.translate.get([
            'base.themes.lightness',
            'base.themes.darkness',
            'base.change-password.title',
            'base.logout']).subscribe((res: Object) => {
                const zh_CN = '中文';
                const en_US = 'English';
                this.langLabels = { en_US, zh_CN  };

                this.currentLang = this.langLabels[this.appConfig.lang];

                this.langs = [
                    {
                        label: zh_CN,
                        command: (event) => {
                            this.switchLang(event, 'zh_CN');
                            event.originalEvent.preventDefault();
                        }
                    },
                    {
                        label: en_US,
                        command: (event) => {
                            this.switchLang(event, 'en_US');
                            event.originalEvent.preventDefault();
                        }
                    }
                ];

                const lightness = res['base.themes.lightness'];
                const darkness = res['base.themes.darkness'];

                this.themeLabels = {
                    lightness, darkness
                };

                this.currentTheme = this.themeLabels[this.appConfig.theme];

                this.themes = [
                    {
                        label: lightness,
                        icon: 'fa-sun-o',
                        command: (event) => {
                            this.switchTheme(event, 'lightness');
                            event.originalEvent.preventDefault();
                        }
                    },
                    {
                        label: darkness,
                        icon: 'fa-moon-o',
                        command: (event) => {
                            this.switchTheme(event, 'darkness');
                            event.originalEvent.preventDefault();
                        }
                    }
                ];

                this.settings = [
                    {
                        label: res['base.change-password.title'],
                        icon: 'fa-key',
                        routerLink: ['/change-password']
                    },
                    {
                        label: res['base.logout'],
                        icon: 'fa-sign-out',
                        command: (event) => {
                            this.logout();
                            event.originalEvent.preventDefault();
                        }
                    }
                ];
            });
    }
    public toHome(){
        this.router.navigate(['/home']);
    }

    private logout() {
        this.authService.logout();
        this.router.navigate(['/login']);
    }

    public toggleMenu(e: Event) {
        this.isMenuHidden = !this.isMenuHidden;
        event.preventDefault();
    }

    @HostListener('document:click') private hideMenu() {
        this.isMenuHidden = true;
    }

    private switchTheme(event: Event, theme: string) {
        this.appConfig.theme = theme;
        this.currentTheme = this.themeLabels[theme];
        let themeLink: HTMLLinkElement = <HTMLLinkElement>document.getElementById('theme-css');
        themeLink.href = 'themes/' + theme + '/theme.css';
        // event.preventDefault();
    }

    private switchLang(event: Event, lang: string) {
        this.appConfig.lang = lang;
        this.currentLang = this.langLabels[lang];
        // event.preventDefault();
    }

}
