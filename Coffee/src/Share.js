import React, { Component } from "react";
// import { Redirect} from "react-router-dom";
import twitter from './images/sharing/twitter.svg';
import facebook from './images/sharing/facebook.svg';
import giphy from './images/sharing/giphy.png';

// This component runs the share page for all gifs
// The base url ./share will not display any gifs
class Share extends Component {
  constructor(props) {
    super(props);

    this.state={
      gif: ""
    }
  }
  // Update shown gif on each refresh
  componentDidMount(){
    this.createQuery();
  }
  render(){
    return (
      <div className="shareBox">
        <img src={this.state.gif} alt="Gif could not be shown" className="gif"/>
        <br/>
        <a href={"https://twitter.com/intent/tweet/?text=Check%20out%20this%20cool%20gif!%20&amp;url="+this.state.gif} target="_blank">
          <img src={twitter} alt="Share button not found" className="button"/>
        </a>
        <a href={"https://facebook.com/sharer/sharer.php?u="+this.state.gif} target="_blank">
          <img src={facebook} alt="Share button not found" className="button"/>
        </a>
        <a href={this.state.gif} target="_blank">
          <img src={giphy} alt="Share button not found" className="button"/>
        </a>
      </div>
    );
  }

  // Grabs url of gif being shared from the url
  createQuery(){
    var image = "";
    image = this.props.location.search.substring(5);
    this.setState({gif: image});
  }
}
export default Share;
