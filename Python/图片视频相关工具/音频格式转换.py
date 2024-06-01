from pydub import AudioSegment

# 加载mp3文件
audio = AudioSegment.from_mp3(r"C:\Users\xing\Downloads\时间煮雨.mp3")

# 导出为wav格式
audio.export("output.wav", format="wav")