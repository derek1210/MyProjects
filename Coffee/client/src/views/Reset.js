import React, { Component } from "react";
import { NavLink, Redirect} from "react-router-dom";
import { withAlert } from 'react-alert';
import '../style/Login.css';

class Reset extends Component {
    constructor(props) {
      super(props);

      this.state ={
        variable: "email",
        old: "",
        new: "",
        realOldEmail: "",
        realOldName: ""
      }

      this.register = this.register.bind(this);
      this.deleteAccount = this.deleteAccount.bind(this);
      this.userInfo = this.userInfo.bind(this);
      this.handleInputChange = this.handleInputChange.bind(this);
    }

    componentDidMount(){
      this.userInfo();
    }
    render(){
      if(!localStorage.getItem("isLoggedIn")){ // If user is not logged in, they can't change things
        return <Redirect to='/login'/>;
      }
        return (
            <div>
                <h1>Reset Account Values</h1>
                <form className="login_form" onSubmit={this.register}>
                <select name="variable" onChange={this.handleInputChange}>
                  <option value="email">Email</option>
                  <option value="password">Password</option>
                  <option value="name">Name</option>
                </select>
                <br />
                <input className="inputForm"
                 placeholder="Old (Not required for email or name)" name="old" required onChange={this.handleInputChange}/>
                <br />
                <input className="inputForm"
                 placeholder="New" name="new" required onChange={this.handleInputChange}/>
                 <br />
                <input type="submit" placeholder="Log In" name="submit" className="loginButton"/>
                <br />
                </form>


                <NavLink to="/login"
                onClick={this.deleteAccount} className="deleteLink">Delete Account</NavLink>
            </div>
        );
    }

    // Change requested values
    register(e){
      e.preventDefault();
      fetch('api/users/' + localStorage.getItem("uid") + '/' + this.state.variable, {
        method: 'PUT',
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          updater: this.state.new,
          old: this.state.old
        })
      }).then(response => {
        if (response.ok) {
          console.log(response);
          this.props.alert.show('Value changed', {timeout: 3000, type: 'success'});
        }
        else{
          this.props.alert.show('Please check input and try again', {timeout: 3000, type: 'error'});
        }
      }).catch(error => console.log(error));
    }

    deleteAccount(){
      fetch('api/users/' + localStorage.getItem("uid"), {
        method: 'DELETE',
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json',
        }
      }).then(response => {
        if (response.ok){
          localStorage.removeItem("uid");
          localStorage.removeItem("isLoggedIn");
          window.location.reload();
        }
      }).catch(error => console.log(error));
    }

    handleInputChange(event){
      this.setState({[event.target.name]: event.target.value});
    }

    userInfo(){
      fetch("/api/users/" + localStorage.getItem("uid"))
      .then(response => {
              if (response.ok) {
                  return response.json()
              } else {
                  throw new Error("No Gifs");
              }
          })
          .then(json => {
              this.setState({realOldName: json["name"]});
              this.setState({realOldEmail: json["email"]});
      })
      .catch(error => console.log(error));
    }
}

export default withAlert(Reset);
