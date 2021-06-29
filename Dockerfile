FROM mcr.microsoft.com/windows/servercore:ltsc2019
WORKDIR /app
RUN MKDIR C:\\temp
COPY bin\\* ./
CMD rdc.exe C:\\temp