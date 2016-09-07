#WOT对照生成
#WOT-Tank-Information-Creater  

##一个从坦克世界客户端文件中生成tanks.json的小程序。  
##用这个json，你可以将录像中的坦克标识符匹配成客户端中显示的名称  
##A small tool that convert WoT Client files to tanks.json.   
##With this Json file, you can match tanks' names in replays to the tanks' name displayed in your cilent!  
  
####tanks.json的格式尽可能的符合  
####https://github.com/Phalynx/WoT-Dossier-Cache-to-JSON/blob/master/tanks.json  
####The format of tanks.json is similar with  
####https://github.com/Phalynx/WoT-Dossier-Cache-to-JSON/blob/master/tanks.json  
  
在tanks.json中，每一个坦克是一个对象，每个对象有8个键  
In tanks.json every tank is an object, every object have 8 keys:  
id:整数    识别统一国家中的不同坦克  
id:int    identify different tanks in a same country  
countryid:整数    识别不同国家 当前已知国家代码：0-ussr 1-germany 2-usa 3-china 4-france 5-gb 6-japan 7-czech 8-sweden  
countryid:int    identify different countries. Known countryid:0-ussr 1-germany 2-usa 3-china 4-france 5-gb 6-japan 7-czech 8-sweden  
title:字符串    用于匹配po文件中的翻译，也是坦克在录像文件中的标识符 坦克在录像中的格式==国家字符串:title  
tilte:string    used to match translations, tanks' name in replays=country sting:title  
icon_orig:字符串    可以用于匹配坦克图标文件，图标名称==国家字符串-icon_orig  
icon_orig:string    tanks' name for icon files, file name=country sting-icon_orig  
tier:整数   坦克等级  
tier:int   tanks' tier  
type:整数   坦克类型 1-LT 2-MT 3-HT 4-TD 5-SP  
type:int   tanks' type 1-LT 2-MT 3-HT 4-TD 5-SPG  
loacl_name:字符串    坦克在客户端中显示的名称，中文客户端，这项就是中文  
loacl_name:string    tanks' name displayed in your cilent.If you are using chinese cilent,you'll get chinese name.  
  
Sorry for my poor english,this program use Chinese as its language.  
If everything works well, you only need to input the path of WoT Cilent like C:\Games\World_of_Tanks, and just press ENTERs.  
When the console reads "完成！ztyzbb于2016.9.6敬上！", it means the process finished well without any critical problem.  
If the console reads "检测到潜在的数据缺失！", it means some tanks miss the loacl_name key!  
If you want to edit the tanks.json, you may replace "}," with "},^r^n" in your text editer, then the file will be more readable.  
