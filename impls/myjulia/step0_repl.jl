
function READ(str)
    str
end

function EVAL(str)
    str
end

function PRINT(str)
    str
end

function rep(str)
    PRINT(EVAL(READ(str)))
end

function main()
    prompt = "user> "
    print(prompt)
    input = readline(keep=true)
    while input != ""
        print(rep(input))
        print(prompt)
        input = readline(keep=true)
    end
end

main()
