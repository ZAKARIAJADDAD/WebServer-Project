html_content = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Animated HTML Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #3498db; /* Set background color to a shade of blue */
            margin: 20px;
            color: #fff; /* Set text color to white */
            text-align: center;
            animation: colorChange 5s infinite alternate; /* Text color change animation */
        }

        h1 {
            color: #ecf0f1; /* Heading color */
            text-align: center;
            animation: bounce 2s infinite; /* Heading bounce animation */
        }

        p {
            color: #ecf0f1; /* Paragraph color */
            text-align: center;
        }

        @keyframes bounce {
            0%, 20%, 50%, 80%, 100% {
                transform: translateY(0);
            }
            40% {
                transform: translateY(-20px);
            }
            60% {
                transform: translateY(-10px);
            }
        }

        @keyframes colorChange {
            0% {
                color: #ffffff;
            }
            50% {
                color: #f39c12; /* Change text color to a shade of orange */
            }
            100% {
                color: #ffffff;
            }
        }
    </style>
</head>
<body>
    <h1>Hello, World!</h1>
    <p>This is a simple HTML page with animations and colors.</p>
</body>
</html>
"""

print(html_content)
