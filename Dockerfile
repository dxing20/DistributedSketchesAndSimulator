FROM gcc:13.2.0
RUN touch .bashrc
RUN echo "PS1='[\[\e[38;5;99m\]\u\[\e[0m\]]{\[\e[38;5;105m\]Debian GNU/Linux 12\[\e[0m\]}(\[\e[38;5;75m\]SimulatorEnv\[\e[0m\])\[\e[38;5;85m\]\w \[\e[38;5;191m\]\$ \[\e[0m\]'" >> ~/.bashrc
WORKDIR /usr/src
COPY . /usr/src
CMD ["bash"]