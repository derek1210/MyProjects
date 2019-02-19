import React from 'react';

const Navigation = ({ onRouteChange, isSignedIn }) => {
    if (isSignedIn) {
      return (
        <nav>
          <p onClick={() => onRouteChange('signin')} className='f3 link dim black underline pa3 pointer'>Sign Out</p>
        </nav>
      );
    } else {
      return (
        <div>
        </div>
      );
    }
}

export default Navigation;