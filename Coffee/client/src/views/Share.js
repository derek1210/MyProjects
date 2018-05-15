import React, { Component } from "react";
import { withAlert } from 'react-alert';
import twitter from '../images/sharing/twitter.svg';
import facebook from '../images/sharing/facebook.svg';
import giphy from '../images/sharing/giphy.png';
import saveCoffee from '../images/logo.svg';
import '../style/Share.css'

class Share extends Component {
  constructor(props) {
    super(props);

    this.state={
      gif: ""
    }
    this.createQuery = this.createQuery.bind(this);
    this.save= this.save.bind(this);
  }
  componentDidMount(){
    window.scrollTo(0, 0);
    this.createQuery();
  }
  render(){

    return (
      <div className="shareBox">
        <img src={this.state.gif} alt="Gif could not be shown" className="gif"/>
        <br/>
        <a href={"https://twitter.com/intent/tweet/?text=Check%20out%20this%20cool%20gif!%20&amp;url="+this.state.gif} target="_blank">
          <img src={twitter} alt="Share button not found" className="shareButton"/>
        </a>
        <a href={"https://facebook.com/sharer/sharer.php?u="+this.state.gif} target="_blank">
          <img src={facebook} alt="Share button not found" className="shareButton"/>
        </a>
        <a href={this.state.gif} target="_blank">
          <img src={giphy} alt="Share button not found" className="shareButton"/>
        </a>
        <a onClick={this.save} href='/'>
          <img src={saveCoffee} alt="Save button not found" className="shareButton" />
        </a>
      </div>
    );
  }

  createQuery(){
    var image = "";
    image = this.props.location.search.substring(5); // Removes the ?gif=
    this.setState({gif: image});
  }

  save(e) {

    e.preventDefault();

    if (localStorage.getItem("isLoggedIn")){

      console.log("type is " + typeof(localStorage.getItem("uid"))+ " and uid is " + localStorage.getItem("uid"));

      fetch('/api/gifs/'+ localStorage.getItem("uid"), {
          method: 'PUT',
          headers: {
              'Accept': 'application/json',
              'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            gif: this.state.gif
          })
      }).then(response => {
            if (response.ok) {
                this.props.alert.show("Selected gif has been saved.", {timeout: 3000, type: 'success'});
                return JSON.stringify({result:"successfully saved"});
            }
            else {
                console.log("Save Failed");
                return("Save Error");
            }
      })

      // Redirect to login page if not logged in
    } else {
      this.props.alert.show("Please log in first to save.", {timeout: 3000, type: 'error'});
    }
  }

}

export default withAlert(Share);
