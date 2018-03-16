import React, { Component } from "react";

class Login extends Component {
    render(){
        var white_text={color : 'white'};
        return (
            <div>
                <h1 style={white_text}>Log In</h1>
                <form action="">
                    <div id="login_form" class="container">
                        <div>
                            <input type="text" placeholder="Enter Username" name="username" required/>
                        </div>

                        <div>
                            <input type="password" placeholder="Enter Password" name="pw" required/>
                        </div>

                        <button type="submit">Login</button>
                        
                        <div>
                            <label style={white_text}>
                                <input type="checkbox" checked="checked" name="remember" /> Remember me
                            </label>
                            
                            <p style={white_text}>Don't have an account? <a href="#">Sign up here</a></p>
                            
                            <div class="forget">
                                <a href="#">Forgot password?</a>
                            </div>
                        </div>
        
                    </div>
                    
                </form>
            </div>
        );
    }
}

export default Login;