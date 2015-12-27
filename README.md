This code competed in the 2015 Zero Robotics season for Paly Robotics Zero. 

# Zero
Zero Robotics is a programming competition run by MIT and NASA that challenges students to create autonomous programs to control SPHERES robots through space. Every year a challenge is released and teams have five weeks to design, develop, and debug their code. Teams start by creating versions of their code to run in a two-dimensional environment. All functional designs move on to the 3D round which places teams on a leaderboard where they will compete their code in an expanded three-dimensional environment. The top teams form alliances of three and advance on to the Alliance round which contains a final, more elaborate version of the game. The top alliances will move on the ISS Final Round and compete their code on SPHERES on the International Space Station. Teams that do not make the ISS Finals compete in the Virtual Round where finalists play a final match on the ISS. For more information about Zero check out zerorobotics.mit.edu

# Spy Spheres
This year's game is SpySpheres and is focused around locating and taking pictures of the opposing SPHERE. Points are scored by taking and uploading pictures to Earth or by acquiring special point block objects. The field is divided into a light, dark, and gray zone which are constantly on the move. Being in the dark zone protects your SPHERE from pictures but will rapidly drain energy. The light zone makes allows for energy acquistion but exposes your SPHERE to picture. The gray zone both exposes the SPHERE and drains energy, making it a major obstacle. Additional game elements include energy blocks to replenish energy, mirrors which can be activated to block and pictures for a set duration, and fuel that limits the movements your SPHERE can make.

# 2D Round Code
Our 2D round code was very simple and uses the built in api functions to capture the three scoring objects. Having scored a single point, it completed its objective and advanced us to the 3d round.

#3D Round Code: L8KI
Our 3D round code, L8KI, sought to minimize the time spent in the light zone through minimal usage of energy and heavy prioritization of the mirrors. L8KI has a linear state machine that advances through various stages of the game, changing its behavior to match the position of the light and dark zones. L8KI was set to move very quickly to secure contested item blocks at the beginning of the game and as such acted in a very fuel-efficient manner for the rest of the game. L8KI placed 57th out of 128 and advanced us to the Alliance round.
