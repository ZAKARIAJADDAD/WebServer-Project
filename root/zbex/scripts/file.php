<?php
session_start();

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    // Handle login logic
    $username = $_POST['username'];
    $password = $_POST['password'];

    // In a real-world scenario, you would validate the username and password against a database
    // For simplicity, let's assume a hardcoded username and password
    $validUsername = 'user';
    $validPassword = 'password';

    if ($username === $validUsername && $password === $validPassword) {
        // Authentication successful
        $_SESSION['username'] = $username;
    } else {
        // Authentication failed
        echo 'Invalid username or password';
    }
} elseif ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['logout'])) {
    // Handle logout logic
    session_unset();
    session_destroy();
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>User Authentication</title>
</head>

<body>
    <?php if (isset($_SESSION['username'])) : ?>
        <div>
            <h1>Hello, <?php echo $_SESSION['username']; ?>!</h1>
            <p>
                Welcome to your profile. You are now logged in.
                <br>
                <a href="?logout">Logout</a>
            </p>
        </div>
    <?php else : ?>
        <div>
            <h1>Login</h1>
            <form method="post">
                <label for="username">Username:</label>
                <input type="text" id="username" name="username" required>
                <br>
                <label for="password">Password:</label>
                <input type="password" id="password" name="password" required>
                <br>
                <input type="submit" value="Login">
            </form>
        </div>
    <?php endif; ?>
</body>

</html>
