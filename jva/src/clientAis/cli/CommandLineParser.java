package clientAis.cli;

import java.util.*;
import java.util.stream.IntStream;
import java.util.stream.Stream;

public class CommandLineParser {

    private final List<String> args;

    public CommandLineParser(String[] args) {
        this.args = Arrays.asList(args);
    }

    public Optional<String> findArgument(String commandName) {
        final List<String> argumentList = findArguments(commandName, 1);

        if(argumentList.size() == 0) {
            return Optional.empty();
        }

        return Optional.of(argumentList.get(0));
    }

    public List<String> findArguments(String commandName, int numberOfArguments) {
        final List<String> arguments = new ArrayList<>();

        args.stream()
                .filter(s -> s.equals(commandName))
                .findFirst()
                .ifPresent(cmdStr -> {
                    final int commandIndex = args.indexOf(cmdStr);
                    IntStream.range(1, numberOfArguments+1).forEach(i -> {
                        final String argument = args.get(commandIndex+i);
                        arguments.add(argument);
                    });
                });

        return arguments;
    }
}
