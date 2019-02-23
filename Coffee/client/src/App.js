import React, { Component } from 'react';
import { Route, NavLink, HashRouter} from "react-router-dom";
import { Provider as AlertProvider } from 'react-alert'
import AlertTemplate from 'react-alert-template-basic'
import logo from './images/logo.svg';
import pwg from './images/pwg.png';
import './style/App.css';
import './style/Footer.css';
import Search from "./views/Search"
import Trending from "./views/Trending"
import Translate from "./views/Translate"
import Profile from "./views/Profile"
import Login from "./views/Login"
import Register from "./views/Register"
import Reset from "./views/Reset"
import Share from "./views/Share"

// Navigation UI. Default view is controlled by Trending.js
class App extends Component {
  render() {
    return (
      <AlertProvider template={AlertTemplate}>
      <HashRouter>
        <div className="App">
          <header className="App-header">
              <img src={logo} className="App-logo" alt="logo" />
              <h1 className="App-title">Coffee</h1>
              <ul className="header">
                <li><NavLink exact to="/">Trending</NavLink></li>
                <li><NavLink to="/search">Search</NavLink></li>
                <li><NavLink to="/translate">Translate</NavLink></li>
                <li><NavLink to="/profile">Profile</NavLink></li>
                <li className="rightHand"><NavLink to="/login" onClick={this.logOut}>Log {this.getStatus()}</NavLink></li>
              </ul>
          </header>
          <div className="content">
            <Route exact path="/" component={Trending}/>
            <Route path="/search" component={Search}/>
            <Route path="/translate" component={Translate}/>
            <Route path="/profile" component={Profile}/>
            <Route path="/login" component={Login}/>
            <Route path="/register" component={Register}/>
            <Route path="/reset" component={Reset}/>
            <Route path="/share" component={Share}/>
          </div>
          <footer className="App-footer">
            <p>
              <span className="l">
                "Coffee" icon by artworkbean from the <a href="https://thenounproject.com/search/?q=coffee&creator=37017&i=43989">Noun Project</a>
              </span>
              <span className="r">
                <img src={pwg} key={pwg} alt="Powered By Giphy" className="foot"/>
              </span>
            </p>
          </footer>
        </div>
      </HashRouter>
      </AlertProvider>
    );
  }

  getStatus(){
    if(localStorage.getItem("isLoggedIn")){
      return 'Out';
    }
    else {
      return 'In';
    }
  }
  logOut(){
    if(localStorage.getItem("isLoggedIn")){
      localStorage.removeItem("uid");
      localStorage.removeItem("isLoggedIn");
      window.location.reload();
    }
  }
}

export default App;
