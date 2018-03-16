import React, { Component } from 'react';
import { Route, NavLink, HashRouter} from "react-router-dom";
import logo from './images/logo.svg';
import pwg from './images/pwg.png';
import './App.css';
import Search from "./Search"
import Trending from "./Trending"
import Translate from "./Translate"
import Profile from "./Profile"
import Login from "./login"
import Share from "./Share"

// Navigation UI. Default view is controlled by Trending.js
class App extends Component {
  render() {
    return (
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
                  <li id="login"><NavLink to="/login">Log In</NavLink></li>
                </ul>
          </header>
          <div className="content">
            <Route exact path="/" component={Trending}/>
            <Route path="/search" component={Search}/>
            <Route path="/translate" component={Translate}/>
            <Route path="/profile" component={Profile}/>
            <Route path="/login" component={Login}/>
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
    );
  }
}

export default App;
