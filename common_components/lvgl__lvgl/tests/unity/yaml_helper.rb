# ==========================================
# Проект Unity — тестовая среда для C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
# [Выпущено под лицензиейMIT. Пожалуйста, обратитесь кlicense.txtдля получения подробной информации]
# ==========================================

require 'yaml'

module YamlHelper
  def self.load(body)
    if YAML.respond_to?(:unsafe_load)
      YAML.unsafe_load(body)
    else
      YAML.load(body)
    end
  end

  def self.load_file(file)
    body = File.read(file)
    self.load(body)
  end
end
