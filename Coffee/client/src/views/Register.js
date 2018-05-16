import React, { Component } from "react";
import { NavLink, Redirect} from "react-router-dom";
import { withAlert } from 'react-alert';
import '../style/Login.css';

class Register extends Component {
    constructor(props) {
      super(props);

      this.state ={
        name: "",
        username: "",
        pw: "",
        repeat: "",
        redirect: false
      }

      this.register = this.register.bind(this);
      this.handleInputChange = this.handleInputChange.bind(this);
    }
    render(){
        const { redirect } = this.state;
        if (redirect || localStorage.getItem("isLoggedIn") ){
          return <Redirect to='/'/>;
        }
        return (
            <div>
                <h1>Register</h1>
                <form className="login_form" onSubmit={this.register}>
                  <input className="inputForm" type="text" autoComplete="name"
                   placeholder="Name" name="name" required onChange={this.handleInputChange}/>
                  <br />
                  <input className="inputForm" type="email" autoComplete="username"
                   placeholder="Email" name="username" required onChange={this.handleInputChange}/>
                  <br />
                  <input className="inputForm" type="password" autoComplete="new-password"
                  placeholder="Password" name="pw" required onChange={this.handleInputChange}/>
                  <br />
                  <input className="inputForm" type="password" autoComplete="new-password"
                  placeholder="Repeat Password" name="repeat" required onChange={this.handleInputChange}/>
                  <br />
                  <input type="submit" placeholder="Log In" name="submit" className="loginButton"/>
                  <br />
                  <div>
                      <p>
                       Already have an account? <NavLink to="/login" className="pwHelp">Log in here</NavLink>
                      </p>
                  </div>
                </form>
            </div>
        );
    }

    // Register a new user
    register(e){
      e.preventDefault();
      if (this.state.pw !== this.state.repeat){
        this.props.alert.show("Passwords do not match", {timeout: 3000, type: 'error'});
      }
      else {
        fetch('api/register/', {
          method: 'POST',
          headers: {
            'Accept': 'application/json',
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({
            email: this.state.username,
            name: this.state.name,
            password: this.state.pw
          })
        })
        .then(response => {
          if (response.ok) {
            localStorage.setItem("isLoggedIn", true);
            return response.json();
          }
          else {
            console.log("Registration Failed");
            return("Registration Error");
          }
        }).then(json => {
          if (localStorage.getItem("isLoggedIn")){
            localStorage.setItem("uid", json);
            window.location.reload();
            this.setState({ redirect: true });
          }
          else {
            this.props.alert.show("User already exists", {timeout: 3000, type: 'error'});
          }
        })
        .catch(error => console.log(error));
      }
    }

    // Change the state
    handleInputChange(event){
      this.setState({[event.target.name]: event.target.value});
    }
}

export default withAlert(Register);
