import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import {MatToolbarModule} from '@angular/material/toolbar';
import {MatButtonModule} from '@angular/material/button';
import {MatCardModule} from '@angular/material/card';
import {MatInputModule} from '@angular/material/input';
import { AppRoutingModule,routingComponents } from './app-routing.module';
import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { StartComponent } from './start/start.component';
import { FreshComponent } from './fresh/fresh.component';
import { DairyComponent } from './dairy/dairy.component';
import { NonpersihableComponent } from './nonpersihable/nonpersihable.component';
import { HometoolsComponent } from './hometools/hometools.component';
import { CleanindproductsComponent } from './cleanindproducts/cleanindproducts.component';

@NgModule({
  declarations: [
    AppComponent,
    routingComponents,
    HomeComponent,
    StartComponent,
    FreshComponent,
    DairyComponent,
    NonpersihableComponent,
    HometoolsComponent,
    CleanindproductsComponent
  ],
  imports: [
    BrowserModule,
    MatButtonModule,
    AppRoutingModule,
    MatToolbarModule,
    MatCardModule,
    MatInputModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
