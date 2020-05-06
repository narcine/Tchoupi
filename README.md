# Tchoupi
Tchoupi is a basic player with interactive subtitles. It will help you to learn a language. 
A easy way to translate a word without opening any translator.

## Prerequisites
If you want to build the source code you need to install Qt :

```
minimum Qt 5.12.0
minimum QtCreator 4.8.1
```

## Installation

```bash
git clone https://github.com/narcine/Tchoupi.git
```
## Run the player
Go inside the binaries folder

```bash
./Tchoupi
```
## How to use it ?

- Click on the open button to choose a video from your computer
- Press the play button.

- Fly over a label, the text color will turn into blue and a tooltip containing the translation will display.
The player will be also paused. If the text color does not turn into blue it means no translation has been provided.

 - Use the mouse, the play button or the space bar to play/pause.
 
![](images/translation_example_1.png)

## How it works ?

### Srt file

The Tchoupi application uses only that format for the subtitles.
Download on the internet and put it next to the video.

```
The **SRT** files follow these rules :
  [subId]
  [start_position] --> [end_position]
  [subtitle_1]
  [subtitle_2]

  [subId]
  [start_position] --> [end_position]
  [subtitle_1]
       ---
  [subtitle_N]

Ex:
  66
  00:10:33,744 --> 00:10:35,945
  ( Sighs )
  ( Laughing )
  
**subId** : The subtitle ID
**start_position** : The starting time
**end_position** : The ending time
```
### Dic file

Dic files can be found in the translation folder. They help to translate words.

If you open one you will see a lot of words with a default translation.

It's possible to create your own dic file. You can do that to override the default translation 
or given a specific translation for a subtitle. Your dic file have to be located in the same place as the video.

```
The **DIC** files follow these rules :
  [words] :
  [subId], [translation_1]
  [subId], [translation_2]
  
  [words] :
  [subId], [translation_1]
    ---
  [subId], [translation_N]

Ex :
  snorts :
  default, renifle
  1, se renifle
  
**words** : Words or group of words encountered in the subtitles
**subId** : The subtitle ID tag or the default tag.
            When the subtitle ID tag is used, the translation will be only applied on the desired subtitle
```

### Mandatory syntax & Location

The srt file, your dic file and the video file must have the same name and located in the same folder.

```
All the files have to end with :
  -[LANG1][LANG2].* 

It means : Translate from LANG1 to LANG2

**LANG1** : The language code (ex : ENG,FR,ESP...) for the first language
**LANG2** : The language code (ex : ENG,FR,ESP...) for the second language

Ex :
  dic-name-ENGFR.dic
  video-name-ENGFR.mkv
```
The dic files found in the translation folder only contain the default translations.
```
Set the environment variable TRANSLATION_PATH to the path where the default translations are located
Ex :
  export TRANSLATION_PATH=../translation
```

```
The dic file has to be named :
  aw_[LANG1][LANG2].dic
```
