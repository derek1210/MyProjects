import React, { Component } from "react";
import { NavLink, Redirect} from "react-router-dom";
import { withAlert } from 'react-alert';
import '../style/Login.css';

class Login extends Component {
    constructor(props) {
      super(props);

      this.state ={
        username: "",
        pw: "",
        remember: false,
        redirect: false
      }

      this.logIn= this.logIn.bind(this);
      this.handleInputChange = this.handleInputChange.bind(this);
    }
    render(){
      this.shouldRedir();
      const { redirect } = this.state;
      if (redirect) {
       return <Redirect to='/'/>;
      }
      else{
        return (
            <div>
                <h1>Log In</h1>
                <form className="login_form" onSubmit={this.logIn}>
                  <input className="inputForm" type="email" autoComplete="username"
                   placeholder="Email" name="username" required onChange={this.handleInputChange}/>
                  <br />
                  <input className="inputForm" type="password" autoComplete="current-password"
                   placeholder="Password" name="pw" required onChange={this.handleInputChange}/>
                  <br />
                  <input type="submit" placeholder="Log In" name="submit" className="loginButton"/>
                  <br />
                  <div>
                      <p> No account? <NavLink to="/register" className="pwHelp">Sign up here</NavLink></p>
                  </div>
                </form>
            </div>
        );
      }
    }

    // Logs the user in
    logIn(e){
      e.preventDefault(); // Prevents auto page refresh
      fetch('api/login/', {
        method: 'POST',
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          email: this.state.username,
          password: this.state.pw
        })
      })
      .then(response => {
        if (response.ok) {
          localStorage.setItem("isLoggedIn", true);
          return response.json();
        }
        return response.text();
      }).then(json => {
        if(localStorage.getItem("isLoggedIn")){
          localStorage.setItem("uid", json);
          window.location.reload();
          this.setState({ redirect: true });
        }
        else{
          this.props.alert.show(json, {timeout: 3000, type: 'error'});
        }
      })
      .catch(error => console.log(error));
    }

    // From the react docs
    handleInputChange(event){
      const value = event.target.type === 'checkbox' ? event.target.checked : event.target.value;
      this.setState({[event.target.name]: value});
    }

    shouldRedir(){
      if(localStorage.getItem("isLoggedIn")){
        this.setState({ redirect: true });
      }
    }
}

export default withAlert(Login);
