import React, { Component } from 'react';
import './FaceRecognition.css';

class FaceRecognition extends Component {
  renderAllboxes() {
    let allBoxes = this.props.box;
    let output = [];
    for (let i = 0; i < allBoxes.length; i++) {
      output.push(<div className='bounding-box' 
      style={{top: allBoxes[i].topRow, 
        right: allBoxes[i].rightCol, 
        bottom: allBoxes[i].bottomRow, 
        left: allBoxes[i].leftCol}}></div>);
    }
    return output;
  }

  render() {
    return (
      <div className='center ma'>
        <div className='absolute mt2'>
          <img id='inputimage' alt='' src={this.props.imageUrl} width='500px' heigh='auto'/>
          {this.renderAllboxes()}
        </div>
      </div>
    );
  }
}

export default FaceRecognition;