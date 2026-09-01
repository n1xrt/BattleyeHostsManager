# BattleyeHostsManager
A utility for playing with people who have the Battleye anti-cheat disabled in GTA Online.
<img width="486" height="413" alt="image" src="https://github.com/user-attachments/assets/bbf0d3e3-a88b-4204-a790-43f733f3c072" />
# For Linux users
If you want to play GTA Online, just add these domains to the hosts file:
`0.0.0.0 paradise-s1.battleye.com`
`0.0.0.0 test-s1.battleye.com`
`0.0.0.0 paradiseenhanced-s1.battleye.com`
After that, add this to the GTA 5 launch options (change the location if you have Proton BattlEye Runtime on a different drive): 
`PROTON_BATTLEYE_RUNTIME=~/.local/share/Steam/steamapps/common/Proton\ BattlEye\ Runtime/ %command%`
After this, GTA Online should work for you.
