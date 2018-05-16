import React, { Component } from "react";
import { NavLink, Redirect} from "react-router-dom";
import '../style/Profile.css';

class Profile extends Component {
    constructor(props) {
        super(props);

        this.state = {
          gifs: [],
          name: ''
        };
        this.grabImages = this.grabImages.bind(this);
        this.userGifs = this.userGifs.bind(this);
        this.userInfo = this.userInfo.bind(this);
    }

    componentDidMount(){
      window.scrollTo(0, 0);
      if(localStorage.getItem("isLoggedIn")){
        this.userGifs();
        this.userInfo();
      }
    }

    render () {
      var parent = this;
      if(!localStorage.getItem("isLoggedIn")){ // If user is not logged in, they can't view their profile
        return <Redirect to='/login'/>;
      }
      
      var renderedGifs = this.state.gifs.map(function (gif, i){
          return (<figure className="savedDisplay" key={i}>
            <NavLink to={'/share?gif='+ gif} key={i} className="gifLink">
              <img src={gif} alt="Gif failed to load" className='gif'/>
            </NavLink><figcaption><NavLink to="/profile" className="deleteLink"
            onClick={parent.deleteGif.bind(null, gif)}>Delete Gif</NavLink></figcaption></figure>)
      });
        return (
            <div>
                <h1>{this.state.name}'s Gifs</h1>
                <div className="gifbox" ref="gifContainer">
                    {renderedGifs}
                </div>
                <br/>
                <br/>
                <br/>
                <NavLink to='/reset' className="deleteLink">Change Account Values</NavLink>
            </div>

        );
    }

    grabImages(data){
        var len = data.length;
        var images = [];
        for (var i = 0; i < len; i++){
          images.push(data[i]);
        }

        this.setState({gifs: images})

    }

    grabInfo(data){
        this.setState({name: data["name"]})
    }

    /* Placeholder. Since we won't have a logged in user until next phase.
       Based on code from class */

       // TODO replace this with a method that calls /api/gifs to get the user's saved gifs

    userGifs() {

        fetch("/api/gifs/" + localStorage.getItem("uid"))
        .then(response => {
                if (response.ok) {
                    return response.json()
                } else {
                    throw new Error("No Gifs");
                }
            })
            .then(json => {

                // console.log("Response ",json)
                this.grabImages(json[0]["gifs"]);
                console.log(json);

        })
        .catch(error => console.log(error))
    }

    deleteGif(gif){
      fetch('api/gifs/' + localStorage.getItem("uid"), {
        method: 'DELETE',
        headers: {
          'Accept': 'application/json',
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          "gif": gif
        })
      }).then(json => {
        console.log("Response ", json);
        window.location.reload();
      }).catch(error => console.log(error));
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
              console.log("Response ",json)
              this.grabInfo(json[0]);


      })
      .catch(error => console.log(error));

    }
}

export default Profile;
