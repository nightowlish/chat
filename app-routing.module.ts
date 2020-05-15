import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import {SigninComponent} from './signin/signin.component';
import {SignupComponent} from './signup/signup.component';
import {PrivacypolicyComponent} from './privacypolicy/privacypolicy.component';
import {HomeComponent} from './home/home.component';
import {FructeComponent} from './fructe/fructe.component';
import {LegumeComponent} from './legume/legume.component';
import {StartComponent} from './start/start.component';
import {FreshComponent} from './fresh/fresh.component';
import {DairyComponent} from './dairy/dairy.component';
import {NonpersihableComponent} from './nonpersihable/nonpersihable.component';
import {HometoolsComponent} from './hometools/hometools.component';
import {CleanindproductsComponent} from './cleanindproducts/cleanindproducts.component';





const routes: Routes = [
	{ path: 'signin',component:SigninComponent},
{path:'signup',component:SignupComponent},
{path:'privacypolicy',component:PrivacypolicyComponent},
{path:'home',component:HomeComponent},
{path:'fructe',component:FructeComponent},
{path:'legume',component:LegumeComponent},
{path:'start',component:StartComponent},
{path:'fresh',component:FreshComponent},
{path:'dairy',component:DairyComponent},
{path:'nonpersihable',component:NonpersihableComponent},
{path:'hometools',component:HometoolsComponent},
{path:'cleanindproducts',component:CleanindproductsComponent}


];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }

export const routingComponents=[SigninComponent,SignupComponent,PrivacypolicyComponent,HomeComponent,FructeComponent,LegumeComponent,StartComponent,FreshComponent,DairyComponent,NonpersihableComponent,HometoolsComponent,CleanindproductsComponent]
